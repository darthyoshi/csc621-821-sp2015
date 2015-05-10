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
}

void Segmentor::SetMovingImage(BaseImage::Pointer movingImage){
  m_subtractFilter->SetInput2(movingImage);
  Segment();
}

void Segmentor::BuildToolbox() {
  m_toolBox = new QWidget();
}

// Run the actual segmentation process here.
void Segmentor::Segment() {
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

  // Set render background.
  m_renderer->GradientBackgroundOn();
  m_renderer->SetBackground(0.7, 0.7, 0.7);
  m_renderer->SetBackground2(0.2, 0.2, 0.2);
  m_renderer->Render();
}

void Segmentor::UpdateView() {}
