#include "DICOMSeries.h"
#include "easylogging++.h"

DICOMSeries::DICOMSeries(const std::string path) : m_path(path) {
  CLOG(INFO, "io") << "Mounted" << m_path << "as DICOM directory.";
}

DICOMSeries::~DICOMSeries() {}

void DICOMSeries::Load() {
  CLOG(INFO, "io") << "Loading" << m_path << "as DICOM data.";

  ImageIOType::Pointer gdcmIO = ImageIOType::New();
  InputNamesGeneratorType::Pointer inputNames = InputNamesGeneratorType::New();
  inputNames->SetInputDirectory(m_path);

  const ReaderType::FileNamesContainer& filenames = inputNames->GetInputFileNames();
  ReaderType::Pointer reader = ReaderType::New();

  reader->SetImageIO(gdcmIO);
  reader->SetFileNames(filenames);

  try {
    reader->Update();
  } catch (itk::ExceptionObject& excp) {
    CLOG(ERROR, "io") << "Exception thrown while reading the series.";
    CLOG(ERROR, "io") << excp.what();
  }

  const auto& region = reader->GetOutput()->GetLargestPossibleRegion();
  m_spacing = reader->GetOutput()->GetSpacing();
  m_size = region.GetSize();

  // Load Metadata
  auto dictionary = (*(reader->GetMetaDataDictionaryArray()))[0];
  itk::ExposeMetaData<std::string>(*dictionary, "0020|000d", m_studyID);
  itk::ExposeMetaData<std::string>(*dictionary, "0008|0016", m_sopUID);

  // Yay! We successfully loaded a DICOM directory.
  CLOG(INFO, "io") << "Loaded" << filenames.size() << "images in series.";
  CLOG(INFO, "io") << "Size:" << m_size;
  CLOG(INFO, "io") << "Spacing:" << m_spacing;

  for (auto x : dictionary->GetKeys()) {
    CLOG(INFO, "io") << x;
  }

}
