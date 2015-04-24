#include "ScalarConnectedComponent.cxx"
#include "itkImage.h"
#include "itkImageFileWriter.h"
#include "itkLabelToRGBImageFilter.h"

int main (int argc, char *argv[])
{
    const unsigned int Dimension = 3;
    typedef short                                  PixelType;
    typedef itk::Image<PixelType, Dimension>       ImageType;
    
    typedef itk::RGBPixel<unsigned char>           RGBPixelType;
    typedef itk::Image<RGBPixelType, Dimension>    RGBImageType;
    
    typedef unsigned int                           LabelPixelType;
    typedef itk::Image<LabelPixelType, Dimension > LabelImageType;
    
    typedef itk::LabelToRGBImageFilter<LabelImageType, RGBImageType> RGBFilterType;
    
    typedef unsigned char                            OutputPixelType;
    
    typedef  itk::ImageFileWriter< RGBImageType > WriterType;
    
    //WriterType::Pointer writer = WriterType::New();
    
    //writer->SetFileName( argv[2] );
    
    ScalarConnectedComponent<ImageType, RGBImageType, LabelImageType> scc = ScalarConnectedComponent<ImageType, RGBImageType, LabelImageType>();
    
    scc.SetImage(argv[1]);
    scc.SetThreshold(4);
    scc.SetMinSize(20);
    scc.SetFilters();
    scc.GetOutput();
    //writer->SetInput(scc.GetOutput());
    
    //writer->Update();
    return 0;
}