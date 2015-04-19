#include "RegWindow.h"

RegWindow::RegWindow(QWidget* parent) : QMainWindow(parent), m_ui(new Ui::RegMainWindow()) {
  m_ui->setupUi(this);
  m_viewLeft = vtkImageViewer2::New();
  m_viewRight = vtkImageViewer2::New();
  readerFixed = Reader::New();
  readerMoving = Reader::New();

  //connect(m_ui->actionOpen_First_Dicom, SIGNAL(triggered()), this, SLOT(LoadDICOMFixed()));
  //connect(m_ui->actionOpen_Second_Dicom, SIGNAL(triggered()), this, SLOT(LoadDICOMMoving()));
  connect(m_ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
}

RegWindow::~RegWindow() {
  delete m_ui;
}

int RegWindow::LoadDICOMFixed() {
  //typedef unsigned short InputPixel;
  //typedef itk::Image<InputPixel, 3> InputImage;

  //typedef itk::ImageSeriesReader<InputImage> Reader;
  //Reader::Pointer reader = Reader::New();

  QDir dir = QFileDialog::getExistingDirectory(0, "Select Folder: ");
  QFileInfoList list = dir.entryInfoList(QDir::Dirs | QDir::Files | 
      QDir::NoDotAndDotDot);

  std::vector<std::string> names;
  foreach (QFileInfo finfo, list) {
    std::string str = dir.path().toStdString().c_str();
    str.append("/");

    names.push_back(str + finfo.fileName().toStdString().c_str());
  }
  readerFixed->SetFileNames(names);

  try {
    readerFixed->Update();
  } catch (itk::ExceptionObject& e) {
    CLOG(INFO, "window") << "Failed to load DICOM file: ";
    return EXIT_FAILURE;
  }

  typedef itk::ImageToVTKImageFilter<InputImage> Connector;
  Connector::Pointer connector = Connector::New();
  connector->SetInput(readerFixed->GetOutput());

  try {
    connector->Update();
  } catch (itk::ExceptionObject& e) {
    CLOG(INFO, "window") << "Failed to convert image data: ";
    return EXIT_FAILURE;
  }

  vtkImageData* image = vtkImageData::New();
  image->DeepCopy(connector->GetOutput());

  m_ui->QVTKMainFixed->SetRenderWindow(m_viewLeft->GetRenderWindow());
  m_viewLeft->SetupInteractor(m_ui->QVTKMainFixed->GetRenderWindow()->GetInteractor());

  m_viewLeft->SetInputData(image);
  m_viewLeft->SetSlice(m_viewLeft->GetSliceMax() / 2);
  m_viewLeft->GetRenderer()->ResetCamera();
  m_viewLeft->Render();

  m_ui->QVTKMainFixed->update();
  return EXIT_SUCCESS;
}

int RegWindow::LoadDICOMMoving() {
  typedef unsigned short InputPixel;
  typedef itk::Image<InputPixel, 3> InputImage;

  //typedef itk::ImageSeriesReader<InputImage> Reader;
  //Reader::Pointer reader = Reader::New();

  QDir dir = QFileDialog::getExistingDirectory(0, "Select Folder: ");
  QFileInfoList list = dir.entryInfoList(QDir::Dirs | QDir::Files | 
      QDir::NoDotAndDotDot);

  std::vector<std::string> names;
  foreach (QFileInfo finfo, list) {
    std::string str = dir.path().toStdString().c_str();
    str.append("/");

    names.push_back(str + finfo.fileName().toStdString().c_str());
  }
  readerMoving->SetFileNames(names);

  try {
    readerMoving->Update();
  } catch (itk::ExceptionObject& e) {
    CLOG(INFO, "window") << "Failed to load DICOM file: ";
    return EXIT_FAILURE;
  }

  typedef itk::ImageToVTKImageFilter<InputImage> Connector;
  Connector::Pointer connector = Connector::New();
  connector->SetInput(readerMoving->GetOutput());

  try {
    connector->Update();
  } catch (itk::ExceptionObject& e) {
    CLOG(INFO, "window") << "Failed to convert image data: ";
    return EXIT_FAILURE;
  }

  vtkImageData* image = vtkImageData::New();
  image->DeepCopy(connector->GetOutput());

  m_ui->QVTKMainMoving->SetRenderWindow(m_viewRight->GetRenderWindow());
  m_viewRight->SetupInteractor(m_ui->QVTKMainMoving->GetRenderWindow()->GetInteractor());

  m_viewRight->SetInputData(image);
  m_viewRight->SetSlice(m_viewRight->GetSliceMax() / 2);
  m_viewRight->GetRenderer()->ResetCamera();
  m_viewRight->Render();

  m_ui->QVTKMainMoving->update();
  return EXIT_SUCCESS;
}

void RegWindow::on_pushButton_OpenFixed_clicked()
{
    LoadDICOMFixed();
}

void RegWindow::on_pushButton_OpenMoving_clicked()
{
    LoadDICOMMoving();
}
