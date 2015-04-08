//The following 3 headers are for testing purposes
//#include "mySubtractImageFilter.cxx"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "Segmentor.cxx"


// main() is based on the example of ThresholdSegmentationLevelSetImageFilter.cxx
// for defineing the pixel type, in this case 2 dimensional, and for reading and writing
int main( int argc, char *argv[] )
{
    if( argc < 4 )
    {
        std::cerr << "Missing Parameters " << std::endl;
        std::cerr << "Usage: " << argv[0];
        std::cerr << " inputImage1 inputImage2 outputImage";
        std::cerr << std::endl;
        return 1;
    }
    
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
    
    reader1->SetFileName( argv[1] );
    reader2->SetFileName( argv[2] );
    writer->SetFileName( argv[3] );
    
    
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