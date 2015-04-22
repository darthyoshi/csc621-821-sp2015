#include "mainWindow.h"
#include "ui_mainWindow.h"
#include "Segmentor.cpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_viewRegFixed = vtkImageViewer2::New();
    this->InitializeRenderWindow(m_viewRegFixed, ui->QVTKRegFixed);

    m_viewRegMoving1 = vtkImageViewer2::New();
    this->InitializeRenderWindow(m_viewRegMoving1, ui->QVTKRegMoving1);

    m_viewRegMoving2 = vtkImageViewer2::New();
    this->InitializeRenderWindow(m_viewRegMoving2, ui->QVTKRegMoving2);

    m_viewSeg1 = vtkImageViewer2::New();
    this->InitializeRenderWindow(m_viewSeg1, ui->QVTKSeg1);

    m_viewSeg2 = vtkImageViewer2::New();
    this->InitializeRenderWindow(m_viewSeg2, ui->QVTKSeg2);

    m_viewCine = vtkImageViewer2::New();
    this->InitializeRenderWindow(m_viewCine, ui->QVTKCineViewer);

    readerRegFixed = Reader::New();
    readerRegMoving1 = Reader::New();
    readerRegMoving2 = Reader::New();
    readerSeg1 = Reader::New();
    readerSeg2 = Reader::New();
    readerCine = Reader::New();

    setTab (tabIdxReg);

    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::InitializeRenderWindow(vtkImageViewer2 *m_view,
        QVTKWidget *qVTKWidget)
{
  qVTKWidget->SetRenderWindow(m_view->GetRenderWindow());
  m_view->SetupInteractor(qVTKWidget->GetRenderWindow()->GetInteractor());
  m_view->GetRenderer()->ResetCamera();
  m_view->Render();
}

void MainWindow::setTab (int tab) {
    ui->tabWidget->setCurrentIndex(tab);
    ui->tabWidget->currentWidget()->raise();
}

int MainWindow::LoadDICOM(
        Reader::Pointer reader,
        vtkImageViewer2 *m_view,
        QVTKWidget *qVTKWidget
        ) {
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


void MainWindow::on_pushButton_SelectRegFixedDir_clicked()
{
    LoadDICOM(readerRegFixed, m_viewRegFixed, ui->QVTKRegFixed);
}

void MainWindow::on_pushButton_SelectRegMoving1Dir_clicked()
{
    LoadDICOM(readerRegMoving1, m_viewRegMoving1, ui->QVTKRegMoving1);
}

void MainWindow::on_pushButton_SelectRegMoving2Dir_clicked()
{
    LoadDICOM(readerRegMoving2, m_viewRegMoving2, ui->QVTKRegMoving2);
}

void MainWindow::on_pushButton_Register_clicked()
{
    //registration logic
    setTab (tabIdxSeg);
}

void MainWindow::on_pushButton_SelectSeg1Dir_clicked()
{
    LoadDICOM(readerSeg1, m_viewSeg1, ui->QVTKSeg1);
}

void MainWindow::on_pushButton_SelectSeg2Dir_clicked()
{
    LoadDICOM(readerSeg2, m_viewSeg2, ui->QVTKSeg2);
}

int MainWindow::convertITKToVTK (
        const itk::Image<unsigned char, 3> * imageITK,
        //OutputImage imageITK,
        vtkImageViewer2 *m_view,
        QVTKWidget *qVTKWidget
        ) {
    Connector::Pointer connector = Connector::New();
    connector->SetInput(imageITK);

    try {
      connector->Update();
    } catch (itk::ExceptionObject& e) {
      CLOG(INFO, "window") << "Failed to convert image data: ";
      return EXIT_FAILURE;
    }

    vtkImageData* imageVTK = vtkImageData::New();
    imageVTK->DeepCopy(connector->GetOutput());

    qVTKWidget->SetRenderWindow(m_view->GetRenderWindow());
    m_view->SetupInteractor(qVTKWidget->GetRenderWindow()->GetInteractor());

    m_view->SetInputData(imageVTK);
    m_view->SetSlice(m_view->GetSliceMax() / 2);
    m_view->GetRenderer()->ResetCamera();
    m_view->Render();

    return EXIT_SUCCESS;
}

void MainWindow::on_pushButton_Segment_clicked()
{
    //typedef   unsigned char   InternalPixelType;
    //const     unsigned int    Dimension = 3;
    //typedef itk::Image< InternalPixelType, Dimension >  myImageType;

    //Segment seg = Segment ();
    //seg.testSeg();

    //This part of code deals deals with the wrapper for the filter
    Segmentor< InputImage > segmentor = Segmentor< InputImage >();
    segmentor.SetInputs( readerSeg1->GetOutput(), readerSeg2->GetOutput() );
    segmentor.SetThreshold(1, 2555);
    convertITKToVTK (segmentor.GetOutput(), m_viewCine, ui->QVTKCineViewer);

    //m_viewCine->SetupInteractor(ui->QVTKCineViewer->GetRenderWindow()->GetInteractor());
    //m_viewCine->SetInputData(segmentor.GetOutput());
    //m_viewCine->SetSlice(m_viewCine->GetSliceMax() / 2);
    //m_viewCine->GetRenderer()->ResetCamera();
    //m_viewCine->Render();

    setTab(tabIdxCine);
}

void MainWindow::on_pushButton_SelectCineDir_clicked()
{
    LoadDICOM(readerCine, m_viewCine, ui->QVTKCineViewer);
}


void MainWindow::on_pushButton_SelectMIPDir_clicked()
{

}

