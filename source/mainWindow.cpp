#include "mainWindow.h"
#include "ui_mainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_viewLeft = vtkImageViewer2::New();
    m_viewRight = vtkImageViewer2::New();
    m_viewSeg = vtkImageViewer2::New();
    readerFixed = Reader::New();
    readerMoving = Reader::New();
    readerSeg = Reader::New();

    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
}

MainWindow::~MainWindow()
{

    delete ui;
}

int MainWindow::LoadDICOMFixed() {
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

  ui->QVTKMainFixed->SetRenderWindow(m_viewLeft->GetRenderWindow());
  m_viewLeft->SetupInteractor(ui->QVTKMainFixed->GetRenderWindow()->GetInteractor());

  m_viewLeft->SetInputData(image);
  m_viewLeft->SetSlice(m_viewLeft->GetSliceMax() / 2);
  m_viewLeft->GetRenderer()->ResetCamera();
  m_viewLeft->Render();

  ui->QVTKMainFixed->update();
  return EXIT_SUCCESS;
}

int MainWindow::LoadDICOMMoving() {
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

  ui->QVTKMainMoving->SetRenderWindow(m_viewRight->GetRenderWindow());
  m_viewRight->SetupInteractor(ui->QVTKMainMoving->GetRenderWindow()->GetInteractor());

  m_viewRight->SetInputData(image);
  m_viewRight->SetSlice(m_viewRight->GetSliceMax() / 2);
  m_viewRight->GetRenderer()->ResetCamera();
  m_viewRight->Render();

  ui->QVTKMainMoving->update();
  return EXIT_SUCCESS;
}

int MainWindow::LoadDICOMSeg() {
  QDir dir = QFileDialog::getExistingDirectory(0, "Select Folder: ");
  QFileInfoList list = dir.entryInfoList(QDir::Dirs | QDir::Files |
      QDir::NoDotAndDotDot);

  std::vector<std::string> names;
  foreach (QFileInfo finfo, list) {
    std::string str = dir.path().toStdString().c_str();
    str.append("/");

    names.push_back(str + finfo.fileName().toStdString().c_str());
  }
  readerSeg->SetFileNames(names);

  try {
    readerSeg->Update();
  } catch (itk::ExceptionObject& e) {
    CLOG(INFO, "window") << "Failed to load DICOM file: ";
    return EXIT_FAILURE;
  }

  typedef itk::ImageToVTKImageFilter<InputImage> Connector;
  Connector::Pointer connector = Connector::New();
  connector->SetInput(readerSeg->GetOutput());

  try {
    connector->Update();
  } catch (itk::ExceptionObject& e) {
    CLOG(INFO, "window") << "Failed to convert image data: ";
    return EXIT_FAILURE;
  }

  vtkImageData* image = vtkImageData::New();
  image->DeepCopy(connector->GetOutput());

  ui->QVTKMain->SetRenderWindow(m_viewSeg->GetRenderWindow());
  m_viewSeg->SetupInteractor(ui->QVTKMain->GetRenderWindow()->GetInteractor());

  m_viewSeg->SetInputData(image);
  m_viewSeg->SetSlice(m_viewSeg->GetSliceMax() / 2);
  m_viewSeg->GetRenderer()->ResetCamera();
  m_viewSeg->Render();

  ui->QVTKMain->update();
  return EXIT_SUCCESS;
}


void MainWindow::on_pushButton_SelectFixedDir_clicked()
{
    LoadDICOMFixed();
}

void MainWindow::on_pushButton_SelectMovingDir_clicked()
{
    LoadDICOMMoving();
}

void MainWindow::on_pushButton_Register_clicked()
{

}

void MainWindow::on_pushButton_Segment_clicked()
{
    Segment seg = Segment ();
    seg.testSeg();
}

void MainWindow::on_pushButton_SelectSegDir_clicked()
{
    LoadDICOMSeg();
}

