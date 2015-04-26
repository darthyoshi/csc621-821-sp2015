#include <Visualize.h>
#include "Loader.h"

using namespace vis;

Loader::Loader() : Stage() {
  m_reader = Reader::New();
  m_converter = Converter::New();
  m_converter->SetInput(m_reader->GetOutput());

  BuildToolbox();
  BuildContent();
}

void Loader::BuildToolbox() {

  m_toolBox = new QWidget();
  m_toolBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
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
  m_toolBox->setLayout(pageLayout);

  connect(loadButton, SIGNAL(clicked()), this, SLOT(LoadDICOMSource()));
}

void Loader::BuildContent() {
  m_view = new QVTKWidget();
  m_viewer = vtkResliceImageViewer::New();

  m_view->SetRenderWindow(m_viewer->GetRenderWindow());
  m_viewer->SetupInteractor(m_view->GetInteractor());

  vtkResliceCursorLineRepresentation* rep = 
    vtkResliceCursorLineRepresentation::SafeDownCast(
        m_viewer->GetResliceCursorWidget()->GetRepresentation());
  rep->GetResliceCursorActor()->GetCursorAlgorithm()->SetReslicePlaneNormal(1);

  m_viewer->SetInputData(m_converter->GetOutput());
  m_viewer->SetSliceOrientation(0);
  m_viewer->SetResliceModeToAxisAligned();
   
  m_renderer = m_viewer->GetRenderer();
  m_renderer->GradientBackgroundOn();
  m_renderer->SetBackground(0.7, 0.7, 0.7);
  m_renderer->SetBackground2(0.2, 0.2, 0.2);
  m_renderer->Render();

}

void Loader::LoadDICOMSource() {

  QDir dir = QFileDialog::getExistingDirectory(0, "Select Folder: ");
  QFileInfoList list = dir.entryInfoList(QDir::Dirs | QDir::Files | 
      QDir::NoDotAndDotDot);

  std::vector<std::string> names;
  foreach (QFileInfo finfo, list) {
    std::string str = dir.path().toStdString().c_str();
    str.append("/");

    names.push_back(str + finfo.fileName().toStdString().c_str());
  }
  m_reader->SetFileNames(names);

  try {
    m_reader->Update();
  } catch (itk::ExceptionObject& e) {
    CLOG(INFO, "window") << "Failed to load DICOM file: ";
    return;
  }

  Reader::DictionaryRawPointer dictionary =
    (*(m_reader->GetMetaDataDictionaryArray()))[0];

  // Expose the study UID metadata value and show it in our interface label.
  std::string studyUID;
  itk::ExposeMetaData<std::string>(*dictionary, "0020|000d", studyUID);
  CLOG(INFO, "loader") << studyUID;

  QString labelText = QString::fromStdString(studyUID);
  QFontMetrics metrics(m_UIDLabel->font());
  m_UIDLabel->setText(metrics.elidedText(labelText, Qt::ElideRight,
    m_UIDLabel->width()));

  UpdateViewer();
  emit SourceChanged(m_reader->GetOutput());
}

QWidget* Loader::GetContent() {
  return m_view;
}

QWidget* Loader::GetToolbox() {
  return m_toolBox;
}

void Loader::UpdateViewer() {
  m_converter->SetInput(m_reader->GetOutput());
  m_converter->Update();
  m_viewer->SetInputData(m_converter->GetOutput());
  m_viewer->Reset();
  m_viewer->GetRenderer()->ResetCamera();
  m_viewer->Render();
}
