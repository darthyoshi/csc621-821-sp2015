#include "Quantifier.h"

using namespace vis;

Quantifier::Quantifier() : Stage() {
  // ITK pipeline elements.
  m_converter = Converter::New();

  // VTK rendering elements.
  m_imageView = vtkImageViewer2::New();
  m_3Dmode = true;

  BuildToolbox();
  BuildContent();
}

void Quantifier::BuildToolbox() {
  //TODO: needs UI stuff for MIP
  //TODO: need to disable vtkInteractorStyle for cine-view

  m_toolBox = new QWidget();
  m_toolBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
  QVBoxLayout* pageLayout = new QVBoxLayout();

  QGroupBox* groupBox = new QGroupBox(tr("Options"));
  QVBoxLayout* layout = new QVBoxLayout();
  QPushButton* toggleButton = new QPushButton(tr("Toggle View Mode"));

  // Details
  QGridLayout* details = new QGridLayout();
  QLabel* nameLabel = new QLabel(tr("Current Mode"));
  QLabel* slicesLabel = new QLabel(tr("<b>Slice:</b>"));

  m_modeLabel = new QLabel(tr(m_labels[0].c_str()));
  m_modeLabel->setAlignment(Qt::AlignRight);
  m_slicesLabel = new QLabel(tr("-"));
  m_slicesLabel->setAlignment(Qt::AlignRight);

  m_sliceSlider = new QSlider();
  m_sliceSlider->hide();
  m_sliceSlider->setTracking(true);

  details->addWidget(m_modeLabel, 1, 1);
  details->addWidget(nameLabel, 1, 0);
  details->addWidget(slicesLabel, 2, 0);
  details->addWidget(m_slicesLabel, 2, 1);
  details->addWidget(m_sliceSlider, 3, 1);

  layout->addWidget(toggleButton);
  layout->addItem(details);
  groupBox->setLayout(layout);

  pageLayout->addWidget(groupBox);
  m_toolBox->setLayout(pageLayout);

  connect(toggleButton, SIGNAL(clicked()), this, SLOT(ToggleMode()));
  connect(m_sliceSlider, SIGNAL(valueChanged(int)), this, SLOT(UpdateSlices()));
}

void Quantifier::BuildContent() {
  // Create the main VTK view.
  m_view = new QVTKWidget();
  m_renderer = vtkRenderer::New();

  // Setup interaction and rendering.
  m_renderer->SetUseDepthPeeling(true);
  m_renderer->SetMaximumNumberOfPeels(50);
  m_renderer->SetOcclusionRatio(0.1);

  //render cine-view
  m_view->SetRenderWindow(m_imageView->GetRenderWindow());
  m_imageView->SetRenderer(m_renderer);
  m_imageView->GetImageActor()->SetVisibility(!m_3Dmode);

  // Set render background.
  m_renderer->GradientBackgroundOn();
  m_renderer->SetBackground(0.7, 0.7, 0.7);
  m_renderer->SetBackground2(0.2, 0.2, 0.2);
  m_renderer->Render();

  //TODO: render MIP
}

void Quantifier::UpdateImage(BaseImage::Pointer image) {
  m_converter->SetInput(image);
  m_converter->Update();

  //TODO: needs to accept blob image if available
  m_imageView->SetInputData(m_converter->GetOutput());

  m_slicesLabel->setText(QString::number(m_currentSlice));

  m_sliceSlider->setMinimum(m_imageView->GetSliceMin());
  m_sliceSlider->setMaximum(m_imageView->GetSliceMax());
  m_currentSlice = m_imageView->GetSliceMin();
}

QWidget* Quantifier::GetContent() {
  return m_view;
}

QWidget* Quantifier::GetToolbox() {
  return m_toolBox;
}

void Quantifier::UpdateSlices() {
  //update cine-view slice
  m_currentSlice = m_sliceSlider->value();

  m_slicesLabel->setText(QString::number(m_currentSlice));

  m_imageView->SetSlice(m_currentSlice);
  m_imageView->Render();
}

void Quantifier::ToggleMode() {
  m_3Dmode = !m_3Dmode;

  //TODO: sidebar needs to be updated when changing modes
  std::string* label;

  if(m_3Dmode) {
    m_sliceSlider->hide();
    label = &m_labels[0];
  }

  else {
    m_sliceSlider->show();
    label = &m_labels[1];
  }
  m_modeLabel->setText(QString::fromStdString(*label));

  //toggle cine-view
  m_imageView->GetImageActor()->SetVisibility(!m_3Dmode);
  m_renderer->ResetCamera();
  m_imageView->Render();

  //TODO: toggle MIP
}
