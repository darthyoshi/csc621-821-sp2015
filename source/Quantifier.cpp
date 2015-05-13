#include <Visualize.h>
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

  m_combo = new QComboBox();
  pageLayout->addWidget(m_combo);

  QGroupBox* groupBox = new QGroupBox(tr("Label Statistics"));
  QGridLayout* layout = new QGridLayout();

  QLabel* id = new QLabel(tr("<b>Component ID:</b>"));
  m_idLabel = new QLabel(tr("-"));
  layout->addWidget(id, 1, 0);
  layout->addWidget(m_idLabel, 1, 1);
  
  QLabel* mean = new QLabel(tr("<b>Mean:</b>"));
  m_meanLabel = new QLabel(tr("-"));
  layout->addWidget(mean, 2, 0);
  layout->addWidget(m_meanLabel, 2, 1);

  QLabel* sigma = new QLabel(tr("<b>Sigma:</b>"));
  m_sigmaLabel = new QLabel(tr("-"));
  layout->addWidget(sigma, 3, 0);
  layout->addWidget(m_sigmaLabel, 3, 1);

  QLabel* variance = new QLabel(tr("<b>Variance:</b>"));
  m_varianceLabel = new QLabel(tr("-"));
  layout->addWidget(variance, 4, 0);
  layout->addWidget(m_varianceLabel, 4, 1);

  QLabel* sum = new QLabel(tr("<b>Sum:</b>"));
  m_sumLabel = new QLabel(tr("-"));
  layout->addWidget(sum, 5, 0);
  layout->addWidget(m_sumLabel, 5, 1);

  QLabel* size = new QLabel(tr("<b>Size:</b>"));
  m_sizeLabel = new QLabel(tr("-"));
  layout->addWidget(size, 6, 0);
  layout->addWidget(m_sizeLabel, 6, 1);

  groupBox->setLayout(layout);
  pageLayout->addWidget(groupBox);
  m_toolBox->setLayout(pageLayout);
}

void Quantifier::UpdateInterface() {
  m_combo->clear();

  typedef typename LabelSet::const_iterator LabelIterator;
  LabelSet labelSet = m_statistics->GetValidLabelValues();
  LabelIterator it = labelSet.begin();

  for (; it != labelSet.end(); ++it) {
    if (!m_statistics->HasLabel(*it)) continue;

    LabelPixel value = *it;

    QPixmap pixmap(30, 30);
    QColor color;
    color.setRed(255.0*m_color->GetRedValue((double)value));
    color.setGreen(255.0*m_color->GetGreenValue((double)value));
    color.setBlue(255.0*m_color->GetBlueValue((double)value));
    pixmap.fill(color);

    QIcon icon(pixmap);

    m_combo->addItem(icon, QString::fromStdString(std::to_string(value)), value);
  }

  connect(m_combo, SIGNAL(activated(int)), this, SLOT(SelectComponent(int)));
}

void Quantifier::SelectComponent(int index) {
  LabelPixel label = m_combo->itemData(index).toUInt();
  if (m_lastSelected != 0 && m_lastSelected != 1) {
    m_opacity->AddPoint((double)m_lastSelected, 0.2, 0.5, 1.0);
  }
  else {
    m_opacity->AddPoint((double)m_lastSelected, 0.0, 0.5, 1.0);
  }

  m_idLabel->setText(QString::number(label));
  m_meanLabel->setText(QString::number(m_statistics->GetMean(label)));
  m_sigmaLabel->setText(QString::number(m_statistics->GetSigma(label)));
  m_varianceLabel->setText(QString::number(m_statistics->GetVariance(label)));
  m_sumLabel->setText(QString::number(m_statistics->GetSum(label)));
  m_sizeLabel->setText(QString::number(m_relabel->GetSizeOfObjectInPhysicalUnits(label)));
  m_opacity->AddPoint((double)label, 1.0, 0.5, 1.0);
  m_view->GetRenderWindow()->Render();
  m_lastSelected = label;
}


void Quantifier::BuildContent() {
  // Create the main VTK view.
  m_view = new QVTKWidget();
  m_renderer = vtkRenderer::New();

  // Setup interaction and rendering.
  m_view->GetRenderWindow()->AddRenderer(m_renderer);
  m_view->GetRenderWindow()->SetAlphaBitPlanes(true);
  m_view->GetRenderWindow()->SetMultiSamples(0);
  m_renderer->SetUseDepthPeeling(true);
  m_renderer->SetMaximumNumberOfPeels(100);
  m_renderer->SetOcclusionRatio(0.1);
  m_interactor = m_view->GetInteractor();

  m_renderer->Render();
  CLOG(INFO, "quant") << "Depth Peeling: " << m_renderer->GetLastRenderingUsedDepthPeeling();

  m_mapper = vtkSmartVolumeMapper::New();
  m_mapper->SetBlendModeToMaximumIntensity();
  m_mapper->SetRequestedRenderModeToRayCast();

  m_color = vtkColorTransferFunction::New();
  m_color->AddRGBPoint(0.0, 0.0, 0.0, 0.0, 0.5, 1.0);
  m_color->AddRGBPoint(1.0, 0.0, 0.0, 0.0, 0.5, 1.0);
  for (unsigned int i = 2; i < 100; i++) {
    double color[3];
    color[0] = (m_rng->GetUniformVariate(0.0, 1.0));
    color[1] = (m_rng->GetUniformVariate(0.0, 1.0));
    color[2] = (m_rng->GetUniformVariate(0.0, 1.0));

    m_color->AddRGBPoint((double)i, 
      color[0], color[1], color[2], 0.5, 1.0);
  }

  m_opacity = vtkPiecewiseFunction::New();
  m_opacity->AddPoint(0.0, 0.0, 0.5, 1.0);
  m_opacity->AddPoint(1.0, 0.0, 0.5, 1.0);
  for (int i = 2; i < 100; i++) {
    m_opacity->AddPoint((double)i, 0.2, 0.5, 1.0);
  }

  m_property = vtkVolumeProperty::New();
  m_property->ShadeOff();
  m_property->SetColor(m_color);
  m_property->SetScalarOpacity(m_opacity);
  m_property->SetInterpolationTypeToNearest();
  m_property->SetScalarOpacityUnitDistance(1.0);
  m_property->SetIndependentComponents(true);

  m_volume = vtkVolume::New();
  m_volume->SetMapper(m_mapper);
  m_volume->SetProperty(m_property);

  m_view->GetRenderWindow()->Render();
  m_interactor->Initialize();
  m_view->GetRenderWindow()->Render();

  // Set render background.
  m_renderer->GradientBackgroundOn();
  m_renderer->SetBackground(0.2, 0.2, 0.2);
  m_renderer->SetBackground2(0.0, 0.0, 0.0);
  m_renderer->AddVolume(m_volume);
  m_view->GetRenderWindow()->Render();
}

void Quantifier::SetImage(BaseImage::Pointer image) {
  m_connector->SetInput(image);
  m_statistics->SetInput(image);
  Quantify();
}

void Quantifier::Quantify() {
  m_statistics->Update();
  UpdateInterface();
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
