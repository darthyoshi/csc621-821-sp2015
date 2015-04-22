#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_viewLeft = vtkImageViewer2::New();
    ui->QVTKRegMoving->SetRenderWindow(m_viewLeft->GetRenderWindow());
    m_viewLeft->SetupInteractor(ui->QVTKRegMoving->GetRenderWindow()->GetInteractor());

    m_viewRight = vtkImageViewer2::New();
    m_viewSeg = vtkImageViewer2::New();
    m_viewCine = vtkImageViewer2::New();
    readerFixed = Reader::New();
    readerMoving = Reader::New();
    readerSeg = Reader::New();
    readerCine = Reader::New();

    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
}

MainWindow::~MainWindow()
{

    delete ui;
}

int MainWindow::LoadDICOM(
        Reader::Pointer reader,
        vtkImageViewer2 *m_view,
        QVTKWidget *qVTKWidget) {
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

  qVTKWidget->SetRenderWindow(m_view->GetRenderWindow());
  m_view->SetupInteractor(qVTKWidget->GetRenderWindow()->GetInteractor());

  m_view->SetInputData(image);
  m_view->SetSlice(m_view->GetSliceMax() / 2);
  m_view->GetRenderer()->ResetCamera();
  m_view->Render();

  qVTKWidget->update();
  return EXIT_SUCCESS;
}


void MainWindow::on_pushButton_SelectFixedDir_clicked()
{
    LoadDICOM(readerFixed, m_viewLeft, ui->QVTKRegFixed);
}

void MainWindow::on_pushButton_SelectMovingDir_clicked()
{
    LoadDICOM(readerMoving, m_viewRight, ui->QVTKRegMoving);
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
    LoadDICOM(readerSeg, m_viewSeg, ui->QVTKSeg);
}


void MainWindow::on_pushButton_SelectCineDir_clicked()
{
    LoadDICOM(readerCine, m_viewCine, ui->QVTKCineViewer);
}


void MainWindow::on_pushButton_SelectMIPDir_clicked()
{

}
