#include <Visualize.h>
#include "Segmentor.h"

using namespace vis;

Segmentor::Segmentor() : Stage() {
  m_subtractFilter = SubtractFilter::New();

  BuildToolbox();
  BuildContent();
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

  // Set render background.
  m_renderer->GradientBackgroundOn();
  m_renderer->SetBackground(0.7, 0.7, 0.7);
  m_renderer->SetBackground2(0.2, 0.2, 0.2);
  m_renderer->Render();
}
