#include <Visualize.h>
#include "Loader.h"

namespace vis {
  class vtkPlaneCallback : public vtkCommand {
    public:
      static vtkPlaneCallback* New() {
        return new vtkPlaneCallback;
      }

      void Execute(vtkObject* caller, unsigned long ev, void* callData) {
        vtkImagePlaneWidget* ipw = dynamic_cast<vtkImagePlaneWidget*>(caller);
        if (ipw) {
          double* wl = static_cast<double*>(callData);

          if (ipw == this->planes[0]) {
            this->planes[1]->SetWindowLevel(wl[0], wl[1]);
            this->planes[2]->SetWindowLevel(wl[0], wl[1]);
          } else if (ipw == this->planes[1]) {
            this->planes[0]->SetWindowLevel(wl[0], wl[1]);
            this->planes[2]->SetWindowLevel(wl[0], wl[1]);
          } else if (ipw == this->planes[2]) {
            this->planes[0]->SetWindowLevel(wl[0], wl[1]);
            this->planes[1]->SetWindowLevel(wl[0], wl[1]);
          }
        }

        for (int i = 0; i < 3; i++) {
          this->planes[i]->UpdatePlacement();
        }
      
        this->planes[0]->GetInteractor()->GetRenderWindow()->Render();
      }

      vtkPlaneCallback() {}
      vtkImagePlaneWidget* planes[3];
  };
}

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
  // Create the main VTK view.
  m_view = new QVTKWidget();
  m_renderer = vtkRenderer::New();

  // Setup interaction and rendering.
  m_view->GetRenderWindow()->AddRenderer(m_renderer);
  m_view->GetRenderWindow()->SetAlphaBitPlanes(true);
  m_view->GetRenderWindow()->SetMultiSamples(0);
  m_renderer->SetUseDepthPeeling(true);
  m_renderer->SetMaximumNumberOfPeels(50);
  m_renderer->SetOcclusionRatio(0.1);
  m_interactor = m_view->GetInteractor();

  // Setup windowing callback.
  vtkPlaneCallback* cb = vtkPlaneCallback::New();
  vtkProperty* prop = vtkProperty::New();
  vtkCellPicker* picker = vtkCellPicker::New();

  // Configure orthogonal planes.
  for (int i = 0; i < 3; i++) {
    m_planes[i] = vtkImagePlaneWidget::New();
    m_planes[i]->SetInteractor(m_interactor);
    m_planes[i]->SetPicker(picker);
    m_planes[i]->RestrictPlaneToVolumeOn();
    
    double color[3] = { 0, 0, 0 };
    color[i] = 1;

    cb->planes[i] = m_planes[i];
    m_planes[i]->AddObserver(vtkCommand::WindowLevelEvent, cb);

    m_planes[i]->GetPlaneProperty()->SetColor(color);
    m_planes[i]->GetPlaneProperty()->BackfaceCullingOff();
    m_planes[i]->GetPlaneProperty()->SetOpacity(0.8);
    m_planes[i]->SetTexturePlaneProperty(prop);
    m_planes[i]->TextureInterpolateOff();
    m_planes[i]->SetResliceInterpolateToLinear();
    m_planes[i]->SetInputData(m_converter->GetOutput());
    m_planes[i]->SetPlaneOrientation(i);
    m_planes[i]->SetSliceIndex(0);
    m_planes[i]->DisplayTextOn();
    m_planes[i]->SetDefaultRenderer(m_renderer);
    m_planes[i]->SetWindowLevel(1358, -27);
    m_planes[i]->On();
    m_planes[i]->InteractionOn();
  }

  // Setup volume rendering.
  m_surface = vtkImageMarchingCubes::New();
  m_surface->SetInputData(m_converter->GetOutput());
  m_surface->ComputeNormalsOn();
  m_surface->ComputeGradientsOff();
  m_surface->SetValue(0, 1000);

  m_mapper = vtkPolyDataMapper::New();
  m_mapper->SetInputConnection(m_surface->GetOutputPort());
  m_mapper->ScalarVisibilityOff();

  vtkActor* actor = vtkActor::New();
  actor->GetProperty()->SetColor(1.0, 0.0, 0.0);
  actor->SetMapper(m_mapper);
  m_renderer->AddActor(actor);

  // Set render background.
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
    CLOG(INFO, "window") << str + finfo.fileName().toStdString();
  }
  m_reader->SetFileNames(names);

  try {
    m_reader->Update();
  } catch (itk::ExceptionObject& e) {
    CLOG(INFO, "window") << "Failed to load DICOM file: " 
      << dir.path().toStdString();
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

  UpdateView();
  emit SourceChanged(m_reader->GetOutput());
}

QWidget* Loader::GetContent() {
  return m_view;
}

QWidget* Loader::GetToolbox() {
  return m_toolBox;
}

void Loader::UpdateView() {
  m_converter->SetInput(m_reader->GetOutput());
  m_converter->Update();

  m_surface->SetInputData(m_converter->GetOutput());
  m_mapper->Update();

  for (int i = 0; i < 3; i++) {
    m_planes[i]->SetInputData(m_converter->GetOutput());
    m_planes[i]->UpdatePlacement();
  }
  m_renderer->ResetCamera();
}
