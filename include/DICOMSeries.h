#ifndef DICOMSERIES_H
#define DICOMSERIES_H

#include <string>
#include <sstream>

#include "itkImage.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkNumericSeriesFileNames.h"
#include "itkImageToVTKImageFilter.h"

#include "itkImageSeriesReader.h"
#include "gdcmUIDGenerator.h"

#include "vtkImageData.h"
#include "vtkSmartPointer.h"

class DICOMSeries {

  public:
    typedef vtkSmartPointer<vtkImageData> Slice;
        
  private:
    typedef signed short PixelType;
    typedef itk::Image<PixelType, 3> ImageType;
    typedef itk::ImageSeriesReader<ImageType> ReaderType;
    typedef itk::GDCMImageIO ImageIOType;
    typedef itk::GDCMSeriesFileNames InputNamesGeneratorType;
    typedef itk::ImageToVTKImageFilter<ImageType> Converter;

  public:
    DICOMSeries(const std::string);
    ~DICOMSeries();

    void Load();
    Slice GetSlice(unsigned int);

  private:
    std::string m_path;
    ImageType::SpacingType m_spacing;
    ImageType::SizeType m_size;
    std::string m_studyID;
    std::string m_sopUID;
    ReaderType::Pointer m_handle;
};

#endif
