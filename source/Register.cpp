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
#include "Register.h"
#include "ui_Register.h"

Register::Register(QWidget* parent) : QMainWindow(parent), m_ui(new Ui::MainWindow()) {
  m_ui->setupUi(this);
  m_view = vtkImageViewer2::New();

  connect(m_ui->actionOpen_First_Dicom, SIGNAL(triggered()), this, SLOT(LoadDICOM()));
  connect(m_ui->actionOpen_Second_Dicom, SIGNAL(triggered()), this, SLOT(LoadDICOM2()));

  connect(m_ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
}

Register::~Register() {
  delete m_ui;
}

int Register::LoadDICOM() {
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

  m_ui->QVTKMain->SetRenderWindow(m_viewLeft->GetRenderWindow());
  m_viewLeft->SetupInteractor(m_ui->QVTKMain->GetRenderWindow()->GetInteractor());

  m_viewLeft->SetInputData(image);
  m_viewLeft->SetSlice(m_view->GetSliceMax() / 2);
  m_viewLeft->GetRenderer()->ResetCamera();
  m_viewLeft->Render();

  m_ui->QVTKMain->update();
  return EXIT_SUCCESS;
}

int Register::LoadDICOM2() {
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

  m_ui->QVTKMain2->SetRenderWindow(m_viewRight->GetRenderWindow());
  m_viewRight->SetupInteractor(m_ui->QVTKMain2->GetRenderWindow()->GetInteractor());

  m_viewRight->SetInputData(image);
  m_viewRight->SetSlice(m_view->GetSliceMax() / 2);
  m_viewRight->GetRenderer()->ResetCamera();
  m_viewRight->Render();

  m_ui->QVTKMain2->update();
  return EXIT_SUCCESS;
}

