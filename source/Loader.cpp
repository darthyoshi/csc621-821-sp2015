#include <Visualize.h>
#include "Loader.h"

using namespace vis;

Loader::Loader(QWidget* parent = 0) : QWidget(parent) {

  /**
   * Setup Interface
   */
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
  QVBoxLayout* pageLayout = new QVBoxLayout();

  QGroupBox* groupBox = new QGroupBox(tr("DICOM Source"));
  QVBoxLayout* layout = new QVBoxLayout();
  QPushButton* loadButton = new QPushButton(tr("Load Source"));

  // Source A Details
  QGridLayout* details = new QGridLayout();
  QLabel* nameLabel = new QLabel(tr("<b>Name:</b>"));
  QLabel* slicesLabel = new QLabel(tr("<b>Slices:</b>"));

  m_UIDLabel = new QLabel(tr("-"));
  m_slicesLabel = new QLabel(tr("-"));
  details->addWidget(nameLabel, 1, 0);
  details->addWidget(m_UIDLabel, 1, 1);
  details->addWidget(slicesLabel, 2, 0);
  details->addWidget(m_slicesLabel, 2, 1);

  layout->addWidget(loadButton);
  layout->addItem(details);
  groupBox->setLayout(layout);

  pageLayout->addWidget(groupBox);
  this->setLayout(pageLayout);

  connect(loadButton, SIGNAL(clicked()), this, SLOT(LoadDICOMSource()));
}

void Loader::LoadDICOMSource() {
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
    return;
  }

  Reader::DictionaryRawPointer dictionary =
    (*(reader->GetMetaDataDictionaryArray()))[0];

  // Expose the study UID metadata value and show it in our interface label.
  std::string studyUID;
  itk::ExposeMetaData<std::string>(*dictionary, "0020|000d", studyUID);
  CLOG(INFO, "loader") << studyUID;

  QString labelText = QString::fromStdString(studyUID);
  QFontMetrics metrics(m_UIDLabel->font());
  m_UIDLabel->setText(metrics.elidedText(labelText, Qt::ElideRight,
    m_UIDLabel->width()));

  m_source = reader;
  emit SourceChanged(m_source->GetOutput());
}
