#include "Segmentor.h"

using namespace vis;

Segmentor::Segmentor() : Stage() {
  // Initialize ITK pipeline objects.
  m_subtract = SubtractFilter::New();
  m_converter = Converter::New();

  m_threshold = ThresholdFilter::New(); 
  m_threshold->SetOutsideValue(std::numeric_limits<signed short>::min());
  m_threshold->SetInsideValue(std::numeric_limits<signed short>::max());
  m_threshold->SetInput(m_subtract->GetOutput());
  m_converter->SetInput(m_threshold->GetOutput());
  
  // Initialize VTK pipeline and view objects.
  m_plane = vtkImagePlaneWidget::New();

  m_firstUpdateView = true;

  BuildToolbox();
  BuildContent();
}

QWidget* Segmentor::GetToolbox() {
  return m_toolBox;
}

QWidget* Segmentor::GetContent() {
  return m_view;
}

void Segmentor::SetFixedImage(BaseImage::Pointer fixedImage){
  m_subtract->SetInput1(fixedImage);
  m_firstUpdateView = true;
}

void Segmentor::SetMovingImage(BaseImage::Pointer movingImage){
  m_subtract->SetInput2(movingImage);
  m_firstUpdateView = true;
  Segment();
}

void Segmentor::BuildToolbox() {
  m_toolBox = new QWidget();
  m_toolBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
  QVBoxLayout* pageLayout = new QVBoxLayout();

  QHBoxLayout* minHeader = new QHBoxLayout();
  QLabel* minSliderLabel = new QLabel(tr("<b>Min Threshold</b>"));
  m_minLabel = new QLabel(tr("-"));
  minHeader->addWidget(minSliderLabel);
  minHeader->addWidget(m_minLabel);
  m_minSlider = new QSlider();
  m_minSlider->setRange((int)std::numeric_limits<signed short>::min(),
      (int)std::numeric_limits<signed short>::max());
  m_minSlider->setOrientation(Qt::Orientation::Horizontal);

  QHBoxLayout* maxHeader = new QHBoxLayout();
  QLabel* maxSliderLabel = new QLabel(tr("<b>Max Threshold</b>"));
  m_maxLabel = new QLabel(tr("-"));
  maxHeader->addWidget(maxSliderLabel);
  maxHeader->addWidget(m_maxLabel);
  m_maxSlider = new QSlider();
  m_maxSlider->setRange((int)std::numeric_limits<signed short>::min(),
      (int)std::numeric_limits<signed short>::max());
  m_maxSlider->setOrientation(Qt::Orientation::Horizontal);

  connect(m_minSlider, &QSlider::valueChanged, [=](int value) {
      auto upperThreshold = m_threshold->GetUpperThreshold();
      if (value > m_threshold->GetUpperThreshold()) {
        m_threshold->SetLowerThreshold(upperThreshold);
        m_minSlider->setValue((int)upperThreshold);
      } else {
        m_threshold->SetLowerThreshold((BasePixel)value);
        UpdateView();
      }

      auto newMin = m_threshold->GetLowerThreshold();
      m_minLabel->setText(tr(std::to_string(newMin).c_str()));
  });
  connect(m_maxSlider, &QSlider::valueChanged, [=](int value) {
      auto lowerThreshold = m_threshold->GetLowerThreshold();
      if (value < m_threshold->GetLowerThreshold()) {
        m_threshold->SetUpperThreshold(lowerThreshold);
        m_maxSlider->setValue((int)lowerThreshold);
      } else {
        m_threshold->SetUpperThreshold((BasePixel)value);
        UpdateView();
      }

      auto newMax = m_threshold->GetUpperThreshold();
      m_maxLabel->setText(tr(std::to_string(newMax).c_str()));
  });

  pageLayout->addLayout(minHeader);
  pageLayout->addWidget(m_minSlider);
  pageLayout->addLayout(maxHeader);
  pageLayout->addWidget(m_maxSlider);

  m_toolBox->setLayout(pageLayout);
}

// Run the actual segmentation process here.
void Segmentor::Segment() {
  m_threshold->Update();
  emit SegmentationComplete(m_threshold->GetOutput());
  UpdateView();
}

void Segmentor::BuildContent() {
  // Build the Qt widget that stores our VTK window inside of it.
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

  vtkProperty* prop = vtkProperty::New();
  vtkCellPicker* picker = vtkCellPicker::New();

  m_plane->SetInteractor(m_interactor);
  m_plane->SetPicker(picker);
  m_plane->RestrictPlaneToVolumeOn();
  m_plane->SetTexturePlaneProperty(prop);
  m_plane->TextureInterpolateOff();
  m_plane->SetResliceInterpolateToLinear();
  m_plane->SetInputData(m_converter->GetOutput());
  m_plane->SetPlaneOrientationToXAxes();
  m_plane->SetSliceIndex(0);
  m_plane->DisplayTextOn();
  m_plane->SetDefaultRenderer(m_renderer);
  m_plane->SetWindowLevel(1358, -27);
  m_plane->On();
  m_plane->InteractionOn();

  // Set render background.
  m_renderer->GradientBackgroundOn();
  m_renderer->SetBackground(0.7, 0.7, 0.7);
  m_renderer->SetBackground2(0.2, 0.2, 0.2);
  m_renderer->Render();
}

void Segmentor::UpdateView() {
  int curr_slice = m_plane->GetSliceIndex();
  m_threshold->Update();
  m_converter->SetInput(m_threshold->GetOutput());
  m_converter->Update();

  m_plane->SetInputData(m_converter->GetOutput());
  if (m_firstUpdateView) {
    m_plane->SetSliceIndex((int)(m_converter->GetOutput()->GetDimensions()[0] / 2.0));
  } else {
    m_plane->SetSliceIndex(curr_slice);
  }

  m_plane->UpdatePlacement();

  if (m_firstUpdateView) {
    m_renderer->ResetCamera();
    m_firstUpdateView = false;
  }
  m_view->GetRenderWindow()->Render();
}
