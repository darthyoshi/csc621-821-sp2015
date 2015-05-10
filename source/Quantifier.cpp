#include "Quantifier.h"

using namespace vis;

Quantifier::Quantifier() : Stage() {
  // ITK pipeline elements.
  m_converter = Converter::New();

  // VTK rendering elements.
  m_imageView = vtkImageViewer2::New();
  m_3Dmode = false;

  BuildToolbox();
  BuildContent();
}

void Quantifier::BuildToolbox() {
  //TODO: needs UI stuff for MIP
  //TODO: needs UI stuff for iterating through cine-view slices

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

  details->addWidget(m_modeLabel, 1, 1);
  details->addWidget(nameLabel, 1, 0);
  details->addWidget(slicesLabel, 2, 0);
  details->addWidget(m_slicesLabel, 2, 1);

  layout->addWidget(toggleButton);
  layout->addItem(details);
  groupBox->setLayout(layout);

  pageLayout->addWidget(groupBox);
  m_toolBox->setLayout(pageLayout);

  connect(toggleButton, SIGNAL(clicked()), this, SLOT(ToggleMode()));
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

  m_currentSlice = (m_imageView->GetSliceMin() + m_imageView->GetSliceMax()) / 2;
  m_slicesLabel->setText(QString::number(m_currentSlice));
}

QWidget* Quantifier::GetContent() {
  return m_view;
}

QWidget* Quantifier::GetToolbox() {
  return m_toolBox;
}

void Quantifier::UpdateView() {
  //update cine-view slice
  if(!m_3Dmode) {
    m_slicesLabel->setText(QString::number(m_currentSlice));

    m_imageView->SetSlice(m_currentSlice);
    m_imageView->Render();
  } else {

  }
}

void Quantifier::ToggleMode() {
  m_3Dmode = !m_3Dmode;

  //TODO: sidebar needs to be updated when changing modes
  m_modeLabel->setText(QString::fromStdString(m_labels[(m_3Dmode?0:1)]));
  
  //toggle cine-view
  m_imageView->GetImageActor()->SetVisibility(!m_3Dmode);
  m_renderer->ResetCamera();
  m_imageView->Render();
}
