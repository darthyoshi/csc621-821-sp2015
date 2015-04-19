#include "SegWindow.h"

SegWindow::SegWindow(QWidget* parent) : QMainWindow(parent), m_ui(new Ui::SegMainWindow()) {
  m_ui->setupUi(this);
  m_view = vtkImageViewer2::New();

  connect(m_ui->actionLoadDICOM, SIGNAL(triggered()), this, SLOT(LoadDICOM()));
  connect(m_ui->actionRunSegmentation, SIGNAL(triggered()), this, SLOT(testSeg()));
  connect(m_ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
}

SegWindow::~SegWindow() {
  delete m_ui;
}

int SegWindow::LoadDICOM() {
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


// testSeg() is based on the example of ThresholdSegmentationLevelSetImageFilter.cxx
// for defineing the pixel type, in this case 2 dimensional, and for reading and writing
int SegWindow::testSeg()
//int testSeg( int argc, char *argv[] )
{
//    if( argc < 4 )
//    {
//        std::cerr << "Missing Parameters " << std::endl;
//        std::cerr << "Usage: " << argv[0];
//        std::cerr << " inputImage1 inputImage2 outputImage";
//        std::cerr << std::endl;
//        return 1;
//    }
    typedef   unsigned char           InternalPixelType;
    const     unsigned int    Dimension = 2;
    typedef itk::Image< InternalPixelType, Dimension >  myImageType1;
    typedef itk::Image< InternalPixelType, Dimension >  myImageType2;
    typedef unsigned char                            OutputPixelType;

    typedef itk::Image< OutputPixelType, Dimension > myImageType3;

    typedef  itk::ImageFileReader< myImageType1 > ReaderType;
    typedef  itk::ImageFileWriter<  myImageType1  > WriterType;

    ReaderType::Pointer reader1 = ReaderType::New();
    ReaderType::Pointer reader2 = ReaderType::New();
    WriterType::Pointer writer = WriterType::New();

    reader1->SetFileName("/users/justinacotter/git/jtc-build/fixed/white.png"); // argv[1] );
    reader2->SetFileName("/users/justinacotter/git/jtc-build/moving/x.png"); // argv[2] );
    writer->SetFileName("/users/justinacotter/git/jtc-build/new.png");  // argv[3] );


    //This part of code deals deals with the wrapper for the filter
    Segmentor< myImageType1 > segmentor = Segmentor< myImageType1 >();
    segmentor.SetInputs( reader1->GetOutput(), reader2->GetOutput() );
    segmentor.SetThreshold(1, 2555);
    writer->SetInput( segmentor.GetOutput() );

    // Back to the testing enviornment
    try {
//        reader1->Update();
//        reader2->Update();
        writer->Update();
    } catch (itk::ExceptionObject & excep ) {
        std::cerr << "Exception caught !" << std::endl;
        std::cerr << excep << std::endl;
    }


    return 0;
}
