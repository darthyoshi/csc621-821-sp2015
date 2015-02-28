#ifndef DICOMSERIES_H
#define DICOMSERIES_H

#include <string>
#include <sstream>

#include "itkImage.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkNumericSeriesFileNames.h"

#include "itkImageSeriesReader.h"
#include "gdcmUIDGenerator.h"

class DICOMSeries {
  typedef signed short PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  typedef itk::GDCMImageIO ImageIOType;
  typedef itk::GDCMSeriesFileNames InputNamesGeneratorType;
  typedef itk::ImageSeriesReader<ImageType> ReaderType;

  public:
    DICOMSeries(const std::string path);
    ~DICOMSeries();

    void Load();

  private:
    std::string m_path;
    ImageType::SpacingType m_spacing;
    ImageType::SizeType m_size;
    std::string m_studyID;
    std::string m_sopUID;
};

#endif
