#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkLabelToRGBImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkLabelStatisticsImageFilter.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkScalarConnectedComponentImageFilter.h"

#include "itksys/SystemTools.hxx"
#include <sstream>

template <typename ImageType, typename RGBImageType, typename LabelImageType>
class ScalarConnectedComponent
{
    //const unsigned int Dimension = 2;
    typedef short                                  PixelType;
    //typedef itk::Image<PixelType, Dimension>       ImageType;
  
    typedef itk::RGBPixel<unsigned char>           RGBPixelType;
    //typedef itk::Image<RGBPixelType, Dimension>    RGBImageType;
    
    typedef unsigned int                           LabelPixelType;
    //typedef itk::Image<LabelPixelType, Dimension > LabelImageType;
    
    typedef itk::ImageFileReader<ImageType> ReaderType;
    
    typedef itk::ScalarConnectedComponentImageFilter <ImageType, LabelImageType >
    ConnectedComponentImageFilterType;

    typedef itk::RelabelComponentImageFilter <LabelImageType, LabelImageType >
    RelabelFilterType;
    
    typedef itk::LabelToRGBImageFilter<LabelImageType, RGBImageType>
    RGBFilterType;
    
    typedef itk::ImageSeriesReader< ImageType >        ReaderType;
    
    typedef itk::GDCMImageIO       ImageIOType;
    
    typedef itk::GDCMSeriesFileNames NamesGeneratorType;
    
private:
    typename ImageType::Pointer image;
    PixelType distanceThreshold = 4;
    typename RelabelFilterType::ObjectSizeType minSize;
    
    typename ReaderType::Pointer reader;
    typename ConnectedComponentImageFilterType::Pointer connected;
    typename RelabelFilterType::Pointer relabel;
    typename RGBFilterType::Pointer rgbFilter;
    
    typename ImageIOType::Pointer dicomIO;
    
    typename NamesGeneratorType::Pointer nameGenerator;
    
public:
    
    ScalarConnectedComponent()
    {
        reader = ReaderType::New();
        
        connected = ConnectedComponentImageFilterType::New();
     
        relabel = RelabelFilterType::New();
        
        rgbFilter = RGBFilterType::New();
        
        dicomIO = ImageIOType::New();
        
        nameGenerator = NamesGeneratorType::New();
    }
    
    void SetImage(char* imgFile)
    {
        reader->SetFileName(imgFile);
        reader->Update();
        image = reader->GetOutput();
    }
    
    void SetFile()
    {
        reader->SetImageIO( dicomIO );
        reader->Update();
        image = reader->GetOutput();
    }
                 
    
    void SetThreshold(int threshold)
    {
        distanceThreshold = static_cast<short>(threshold);
    }
    
    void SetMinSize(int size)
    {
        minSize = size;
    }
    
    void SetFilters()
    {
        connected->SetInput(image);
        connected->SetDistanceThreshold(distanceThreshold);
        
        relabel->SetInput(connected->GetOutput());
        relabel->SetMinimumObjectSize(minSize);
        relabel->Update();
        
    }
    /*
    void SetConnectedComponentImageFilterType()
    {
        connected->SetInput();
        connected->SetDistanceThreshold(distanceThreshold);
    }
    
    void SetRelabelComponentImageFilter()
    {
        relabel->SetInput(connected->GetOut)
        relabel->SetMinimumObjectSize(minSize);
        relabel->Update();
        
        SetLabelToRGBImageFilter();
    }*/
    
    RGBImageType *GetOutput()
    {
        SummarizeLabelStatistics (image.GetPointer(), relabel->GetOutput());
        
        rgbFilter->SetInput( relabel->GetOutput() );
        rgbFilter->GetOutput();
    }
    
    
    
    template<typename TImage, typename TLabelImage>
    void SummarizeLabelStatistics (TImage* image, TLabelImage* labelImage)
    {
        typedef itk::LabelStatisticsImageFilter< TImage, TLabelImage >
        LabelStatisticsImageFilterType;
        typename LabelStatisticsImageFilterType::Pointer labelStatisticsImageFilter =
        LabelStatisticsImageFilterType::New();
        labelStatisticsImageFilter->SetLabelInput( labelImage );
        labelStatisticsImageFilter->SetInput(image);
        labelStatisticsImageFilter->UseHistogramsOn(); // needed to compute median
        labelStatisticsImageFilter->Update();
        
        std::cout << "Number of labels: "
        << labelStatisticsImageFilter->GetNumberOfLabels() << std::endl;
        std::cout << std::endl;
        
        typedef typename LabelStatisticsImageFilterType::ValidLabelValuesContainerType ValidLabelValuesType;
        typedef typename LabelStatisticsImageFilterType::LabelPixelType LabelPixelType2;
        
        for(typename ValidLabelValuesType::const_iterator vIt = labelStatisticsImageFilter->GetValidLabelValues().begin();
            vIt != labelStatisticsImageFilter->GetValidLabelValues().end();
            ++vIt)
        {
            if ( labelStatisticsImageFilter->HasLabel(*vIt) )
            {
                LabelPixelType labelValue = *vIt;
                std::cout << "Label: " << *vIt << std::endl;
                std::cout << "\tmin: "
                << labelStatisticsImageFilter->GetMinimum( labelValue )
                << std::endl;
                std::cout << "\tmax: "
                << labelStatisticsImageFilter->GetMaximum( labelValue )
                << std::endl;
                std::cout << "\tmedian: "
                << labelStatisticsImageFilter->GetMedian( labelValue )
                << std::endl;
                std::cout << "\tmean: "
                << labelStatisticsImageFilter->GetMean( labelValue )
                << std::endl;
                std::cout << "\tsigma: "
                << labelStatisticsImageFilter->GetSigma( labelValue )
                << std::endl;
                std::cout << "\tvariance: "
                << labelStatisticsImageFilter->GetVariance( labelValue )
                << std::endl;
                std::cout << "\tsum: "
                << labelStatisticsImageFilter->GetSum( labelValue )
                << std::endl;
                std::cout << "\tcount: "
                << labelStatisticsImageFilter->GetCount( labelValue )
                << std::endl;
                std::cout << "\tregion: "
                << labelStatisticsImageFilter->GetRegion( labelValue )
                << std::endl;
            }
        }
    }

};

