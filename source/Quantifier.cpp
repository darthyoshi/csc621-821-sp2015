#include "Quantifier.h"

using namespace vis;

Quantifier::Quantifier() : Stage() {
  // ITK pipeline elements.
  m_converter = Converter::New();
  m_connector = BlobDetector::New();
  m_relabel = RelabelFilter::New();
  m_rgbFilter = RGBFilter::New();

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
  QLabel* thresholdLabel = new QLabel(tr("<b>Distance Threshold</b>"));
  QLabel* sizeLabel = new QLabel(tr("<b>Minimum Size</b>"));

  m_modeLabel = new QLabel(tr(m_labels[0].c_str()));
  m_modeLabel->setAlignment(Qt::AlignRight);

  m_sliceLabel = new QLabel(tr("<b>Slice:</b>"));
  m_sliceValueLabel = new QLabel(tr("0"));
  m_sliceValueLabel->setAlignment(Qt::AlignRight);

  m_sizeLabel = new QLabel(tr("4"));
  m_sizeLabel->setAlignment(Qt::AlignRight);

  m_thresholdLabel = new QLabel(tr("4"));
  m_thresholdLabel->setAlignment(Qt::AlignRight);

  m_sliceSlider = new QSlider();
  m_sliceSlider->hide();
  m_sliceSlider->setTracking(true);

  m_thresholdSlider = new QSlider();
  m_thresholdSlider->setTracking(false);
  m_thresholdSlider->setValue(4);

  m_sizeSlider = new QSlider();
  m_sizeSlider->setTracking(false);
  m_sizeSlider->setValue(4);

  details->addWidget(m_modeLabel, 1, 1);
  details->addWidget(nameLabel, 1, 0);

  details->addWidget(sizeLabel, 2, 0);
  details->addWidget(m_sizeLabel, 2, 1);
  details->addWidget(m_sizeSlider, 3, 1);

  details->addWidget(thresholdLabel, 4, 0);
  details->addWidget(m_thresholdLabel, 4, 1);
  details->addWidget(m_thresholdSlider, 5, 1);

  details->addWidget(m_sliceLabel, 6, 0);
  details->addWidget(m_sliceValueLabel, 6, 1);
  details->addWidget(m_sliceSlider, 7, 1);

  layout->addWidget(toggleButton);
  layout->addItem(details);
  groupBox->setLayout(layout);

  pageLayout->addWidget(groupBox);
  m_toolBox->setLayout(pageLayout);

  connect(toggleButton, SIGNAL(clicked()), this, SLOT(ToggleMode()));
  connect(m_sliceSlider, SIGNAL(valueChanged(int)), this, SLOT(UpdateSlice()));
  connect(m_sizeSlider, SIGNAL(valueChanged(int)), this, SLOT(UpdateSize()));
  connect(m_thresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(UpdateThreshold()));
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

void Quantifier::SetImage(BaseImage::Pointer image) {
  //set up blob detector
  m_connector->SetInput(image);
  m_connector->SetDistanceThreshold(4);

  //set up relabeler
  m_relabel->SetInput(m_connector->GetOutput());
  m_relabel->SetMinimumObjectSize(4);

  //set up rgb filter
  m_rgbFilter->SetInput(m_relabel->GetOutput());

  //convert to ITK image to VTK image
  m_converter->SetInput(m_rgbFilter->GetOutput());
  m_converter->Update();

  //set up cine-view
  m_imageView->SetInputData(m_converter->GetOutput());
  int currentSlice = m_imageView->GetSliceMin();
  m_sliceSlider->setMinimum(currentSlice);
  m_sliceSlider->setMaximum(m_imageView->GetSliceMax());
  m_sliceValueLabel->setText(QString::number(currentSlice));
}

QWidget* Quantifier::GetContent() {
  return m_view;
}

QWidget* Quantifier::GetToolbox() {
  return m_toolBox;
}

void Quantifier::UpdateSlice() {
  //update cine-view slice
  int currentSlice = m_sliceSlider->value();

  m_sliceValueLabel->setText(QString::number(currentSlice));

  m_imageView->SetSlice(currentSlice);
  m_imageView->Render();
}

void Quantifier::ToggleMode() {
  m_3Dmode = !m_3Dmode;

  //TODO: sidebar needs to be updated when changing modes
  std::string* label;

  if(m_3Dmode) {
    m_sliceSlider->hide();
    m_sliceLabel->hide();
    m_sliceValueLabel->hide();
    label = &m_labels[0];
  }

  else {
    m_sliceSlider->show();
    m_sliceLabel->show();
    m_sliceValueLabel->show();
    label = &m_labels[1];
  }
  m_modeLabel->setText(QString::fromStdString(*label));

  //toggle cine-view
  m_imageView->GetImageActor()->SetVisibility(!m_3Dmode);
  m_renderer->ResetCamera();
  m_imageView->Render();

  //TODO: toggle MIP
}

void Quantifier::UpdateThreshold() {
  short threshold = (short)(m_thresholdSlider->value());

  m_connector->SetDistanceThreshold(threshold);

  m_relabel->Update();
  m_imageView->Render();

  m_thresholdLabel->setText(QString::number(threshold));
}

void Quantifier::UpdateSize() {
  int minSize = m_sizeSlider->value();

  m_relabel->SetMinimumObjectSize(minSize);

  m_relabel->Update();
  m_imageView->Render();

  m_sizeLabel->setText(QString::number(minSize));
}
