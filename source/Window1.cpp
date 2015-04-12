#include <QFileDialog>
#include <QFileInfoList>
#include <QDir>
#include <iostream>

#include <itkImage.h>
#include <itkImageSeriesReader.h>
#include <itkImageToVTKImageFilter.h>

#include <vtkImageViewer2.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include "easylogging++.h"
#include "Window1.h"
#include "ui_Window1.h"

Window1::Window1(QWidget* parent) : QMainWindow(parent), m_ui(new Ui::MainWindow()) {
  m_ui->setupUi(this);
  m_view = vtkImageViewer2::New();

  connect(m_ui->actionLoadDICOM, SIGNAL(triggered()), this, SLOT(LoadDICOM()));
  connect(m_ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
}

Window1::~Window1() {
  delete m_ui;
}

int Window1::LoadDICOM() {
  typedef unsigned short InputPixel;
  typedef itk::Image<InputPixel, 3> InputImage;

  typedef itk::ImageSeriesReader<InputImage> Reader;
  Reader::Pointer reader = Reader::New();

  QDir dir = QFileDialog::getExistingDirectory(0, "Select Folder: ");
  QFileInfoList list = dir.entryInfoList(QDir::Dirs | QDir::Files | 
      QDir::NoDotAndDotDot);

  std::vector<std::string> names;
  foreach (QFileInfo finfo, list) {
    std::string str = dir.path().toStdString().c_str();
    str.append("/");

    names.push_back(str + finfo.fileName().toStdString().c_str());
  }
  reader->SetFileNames(names);

  try {
    reader->Update();
  } catch (itk::ExceptionObject& e) {
    CLOG(INFO, "window") << "Failed to load DICOM file: ";
    return EXIT_FAILURE;
  }

  typedef itk::ImageToVTKImageFilter<InputImage> Connector;
  Connector::Pointer connector = Connector::New();
  connector->SetInput(reader->GetOutput());

  try {
    connector->Update();
  } catch (itk::ExceptionObject& e) {
    CLOG(INFO, "window") << "Failed to convert image data: ";
    return EXIT_FAILURE;
  }

  vtkImageData* image = vtkImageData::New();
  image->DeepCopy(connector->GetOutput());

  m_ui->QVTKMain->SetRenderWindow(m_view->GetRenderWindow());
  m_view->SetupInteractor(m_ui->QVTKMain->GetRenderWindow()->GetInteractor());

  m_view->SetInputData(image);
  m_view->SetSlice(m_view->GetSliceMax() / 2);
  m_view->GetRenderer()->ResetCamera();
  m_view->Render();

  m_ui->QVTKMain->update();
  return EXIT_SUCCESS;
}

