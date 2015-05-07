/***************************************************************************
 * Name: Eric Chu
 * Course: CSC621 - BioMedical Imaging Analysis
 * Assignment: Final Project - Group Wind
 * 
 * Quantification - Connected Component Analysis
 * Description: This class takes in a set of DICOM images or a singular DICOM image
 * and analyzes the connect components of it. The input can be 3D images or 2D
 * images and filters are placed over it to indicate the separate components.
 * Data such as size, region, variance, and median are also calculated for each
 * component.
 ****************************************************************************/

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
#include <string>
#include <sstream>

template <typename ImageType, typename RGBImageType, typename LabelImageType>
class ScalarConnectedComponent
{
    //these variables are used to create the type of images
    typedef short                                  PixelType;
    typedef itk::RGBPixel<unsigned char>           RGBPixelType;
    typedef unsigned int                           LabelPixelType;
    
    //variable used to read image files
    typedef itk::ImageFileReader<ImageType> ReaderType;
    
    //variable used to create each connected component
    typedef itk::ScalarConnectedComponentImageFilter <ImageType, LabelImageType >
    ConnectedComponentImageFilterType;

    //variable used to create data for each connected component
    typedef itk::RelabelComponentImageFilter <LabelImageType, LabelImageType >
    RelabelFilterType;
    
    //variable used to create the colored filters to show each connected component
    typedef itk::LabelToRGBImageFilter<LabelImageType, RGBImageType>
    RGBFilterType;
    
    //variable used to read a series of DICOM images
    typedef itk::ImageSeriesReader< ImageType > SeriesReaderType;
    
    //these variables are used to create the IDs of the generated DICOM images
    typedef itk::GDCMImageIO ImageIOType;
    typedef itk::GDCMSeriesFileNames NamesGeneratorType;
    typedef std::vector< std::string >    SeriesIdContainer;
    typedef std::vector< std::string >   FileNamesContainer;
    
    
private:
    
    //general
    typename ImageType::Pointer image;
    
    PixelType distanceThreshold = 4;
    
    typename ReaderType::Pointer reader;
    
    //filters
    typename RelabelFilterType::ObjectSizeType minSize;
    
    typename ConnectedComponentImageFilterType::Pointer connected;
    
    typename RelabelFilterType::Pointer relabel;
    
    typename RGBFilterType::Pointer rgbFilter;
    
    //series
    typename SeriesReaderType::Pointer seriesReader;
    
    typename ImageIOType::Pointer dicomIO;
    
    typename NamesGeneratorType::Pointer nameGenerator;
    
public:
    
    //default constructor to initialize all pointers
    ScalarConnectedComponent()
    {
        reader = ReaderType::New();
        
        connected = ConnectedComponentImageFilterType::New();
     
        relabel = RelabelFilterType::New();
        
        rgbFilter = RGBFilterType::New();
        
        seriesReader = SeriesReaderType::New();
        
        dicomIO = ImageIOType::New();
        
        nameGenerator = NamesGeneratorType::New();
        
        
    }
    
    //SetImage reads in a single image file
    void SetImage(char* imgFile)
    {
        reader->SetFileName(imgFile);
        reader->Update();
        image = reader->GetOutput();
    }
    
    //SetFile reads in a folder containing a set of images
    void SetFile(const std::string path)
    {
        //generates a name for the series for images
        nameGenerator->SetUseSeriesDetails(true);
        nameGenerator->SetDirectory(path);
        
        const SeriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();
        SeriesIdContainer::const_iterator seriesItr = seriesUID.begin();
        SeriesIdContainer::const_iterator seriesEnd = seriesUID.end();
        
        //generates a ID for each image
        while( seriesItr != seriesEnd )
        {
            std::cout << seriesItr->c_str() << std::endl;
            ++seriesItr;
        }
        
        std::string seriesIdentifier;
        
        seriesIdentifier = seriesUID.begin()->c_str();
        
        FileNamesContainer fileNames;
        
        fileNames = nameGenerator->GetFileNames( seriesIdentifier );
        
        seriesReader->SetImageIO( dicomIO );
        seriesReader->SetFileNames(fileNames);
        seriesReader->Update();
        image = seriesReader->GetOutput();
    }
                 
    //sets the threshold size for each connected component
    void SetThreshold(int threshold)
    {
        distanceThreshold = static_cast<short>(threshold);
    }
    
    //sets the minimum size of each connected component
    void SetMinSize(int size)
    {
        minSize = size;
    }
    
    //sets the filters on each connected component
    void SetFilters()
    {
        connected->SetInput(image);
        connected->SetDistanceThreshold(distanceThreshold);
        
        relabel->SetInput(connected->GetOutput());
        relabel->SetMinimumObjectSize(minSize);
        relabel->Update();
        
    }
    
    //returns a image file specified by the user or program
    RGBImageType *GetOutput()
    {
        SummarizeLabelStatistics (image.GetPointer(), relabel->GetOutput());
        
        rgbFilter->SetInput( relabel->GetOutput() );
        rgbFilter->GetOutput();
    }
    
    //this function outputs the statistics of each connected component
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

