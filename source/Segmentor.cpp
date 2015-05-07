#include <Visualize.h>
#include "Segmentor.h"

using namespace vis;

Segmentor::Segmentor() : Stage() {
	m_subtractFilter = SubtractFilter::New();

  BuildToolbox();
  BuiltContent();
}

QWidget* Segmentor::GetToolbox() {
  return new QWidget();
}

QWidget* Segmentor::GetContent() {
  return new QWidget();
}

void Segmentor::SetFixedImage(BaseImage::Pointer fixedImage){
    m_subtractFilter->SetInput1(fixedImage);

    if(m_subtractFilter->GetNumberOfValidRequiredInputs() == 0 ) {
        //update view in window
        //m_reader->GetOutput();
    }
}

void Segmentor::SetMovingImage(BaseImage::Pointer movingImage){
    m_subtractFilter->SetInput2(movingImage);

    if(m_subtractFilter->GetNumberOfValidRequiredInputs() == 0 ) {
        //update view in window
        //m_reader->GetOutput();
    }
}

//copied from Loader.cpp - probably needs fixing
void Segmentor::BuildToolbox() {

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

//copied from Loader.cpp - probably needs fixing
void Segmentor::BuildContent() {
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
