#include "Quantifier.h"

using namespace vis;

Quantifier::Quantifier() : Stage() {
  // ITK pipeline elements.
  m_converter = Converter::New();
  m_connector = BlobDetector::New();
  m_relabel = RelabelFilter::New();
  m_statistics = LabelStatistics::New();
  m_rng = VariateGenerator::New();

  m_relabel->SetInput(m_connector->GetOutput());
  m_statistics->SetLabelInput(m_relabel->GetOutput());
  m_converter->SetInput(m_relabel->GetOutput());

  m_connector->SetBackgroundValue(0);
  m_connector->SetDistanceThreshold(4);
  m_relabel->SetMinimumObjectSize(10);
  m_statistics->UseHistogramsOn();

  BuildToolbox();
  BuildContent();
}

void Quantifier::BuildToolbox() {
  //TODO: needs UI stuff for MIP

  m_toolBox = new QWidget();
  m_toolBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
  QVBoxLayout* pageLayout = new QVBoxLayout();

  m_toolBox->setLayout(pageLayout);
}

void Quantifier::BuildContent() {
  // Create the main VTK view.
  m_view = new QVTKWidget();
  m_renderer = vtkRenderer::New();

  // Setup interaction and rendering.
  m_view->GetRenderWindow()->AddRenderer(m_renderer);
  m_interactor = m_view->GetInteractor();

  m_mapper = vtkSmartVolumeMapper::New();
  m_mapper->SetBlendModeToMaximumIntensity();
  m_mapper->SetRequestedRenderModeToRayCast();

  vtkColorTransferFunction* colorFunc = vtkColorTransferFunction::New();
  colorFunc->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
  for (unsigned int i = 1; i < 100; i++) {
    double color[3];
    color[0] = m_rng->GetUniformVariate(0.0, 255.0);
    color[1] = m_rng->GetUniformVariate(0.0, 255.0);
    color[2] = m_rng->GetUniformVariate(0.0, 255.0);

    colorFunc->AddRGBPoint((double)i, 
      color[0], color[1], color[2]);
  }

  vtkPiecewiseFunction* opacityFunc = vtkPiecewiseFunction::New();
  opacityFunc->AddSegment(0.0, 0.0, 1.0, 0.0);
  opacityFunc->AddSegment(1.1, 0.2, 100.0, 0.5);

  m_property = vtkVolumeProperty::New();
  m_property->ShadeOff();
  m_property->SetColor(colorFunc);
  m_property->SetScalarOpacity(opacityFunc);
  m_property->SetInterpolationTypeToNearest();
  m_property->SetIndependentComponents(true);

  m_volume = vtkVolume::New();
  m_volume->SetMapper(m_mapper);
  m_volume->SetProperty(m_property);

  // Set render background.
  m_renderer->GradientBackgroundOn();
  m_renderer->SetBackground(0.2, 0.2, 0.2);
  m_renderer->SetBackground2(0.0, 0.0, 0.0);
  m_renderer->AddVolume(m_volume);
  m_renderer->ResetCamera();
  m_renderer->Render();
}

void Quantifier::SetImage(BaseImage::Pointer image) {
  m_connector->SetInput(image);
  m_statistics->SetInput(image);
  Quantify();
}

void Quantifier::Quantify() {
  m_statistics->Update();
  UpdateView();
}

void Quantifier::UpdateView() {
  m_converter->SetInput(m_relabel->GetOutput());
  m_converter->Update();
  m_converter->GetOutput()->PrintSelf(std::cout, vtkIndent());
  
  CLOG(INFO, "quant") << "Label Count: "
    << std::to_string(m_statistics->GetNumberOfLabels());

  m_mapper->SetInputData(m_converter->GetOutput());
  m_mapper->Update();

  m_renderer->ResetCamera();
  m_view->GetRenderWindow()->Render();
}

QWidget* Quantifier::GetContent() {
  return m_view;
}

QWidget* Quantifier::GetToolbox() {
  return m_toolBox;
}
