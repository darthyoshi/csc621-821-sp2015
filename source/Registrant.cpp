#include "Registrant.h"

using namespace vis;

Registrant::Registrant() : Stage() {
  m_reader = Reader::New();
  m_gdcmIO = ImageIOType::New();
  m_namesGenerator = NamesGeneratorType::New();

  // Initialize all registration fields.
  m_transform = Transform::New();
  m_optimizer = Optimizer::New();
  m_interpolator = Interpolator::New();
  m_registration = Registration::New();
  m_metric = Metric::New();

  m_fixedImagePyramid = ImagePyramid::New();
  m_movingImagePyramid = ImagePyramid::New();

  m_registration->SetOptimizer(m_optimizer);
  m_registration->SetTransform(m_transform);
  m_registration->SetInterpolator(m_interpolator);
  m_registration->SetMetric(m_metric);
  m_registration->SetFixedImagePyramid(m_fixedImagePyramid);
  m_registration->SetMovingImagePyramid(m_movingImagePyramid);

  // Cast input (BaseImage) to internal type (InternalImage).
  m_fixedCaster = CastFilter::New();
  m_movingCaster = CastFilter::New();
  m_movingCaster->SetInput(m_reader->GetOutput());
  m_registration->SetFixedImage(m_fixedCaster->GetOutput());
  m_registration->SetMovingImage(m_movingCaster->GetOutput());

  // Create filters for post-registration.
  m_finalTransform = Transform::New();
  m_resample = ResampleFilter::New();
  m_histmatch = HistogramMatchingFilter::New();
  m_histmatch->SetSourceImage(m_resample->GetOutput());
  m_checkerBoard = CheckerBoardFilter::New();
  m_converter = Converter::New();
  m_converter->SetInput(m_checkerBoard->GetOutput());
  
  BuildToolbox();
  BuildContent();
}

void Registrant::BuildToolbox() {
  m_toolBox = new QWidget();
  m_toolBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
  QVBoxLayout* pageLayout = new QVBoxLayout();

  QGroupBox* loadingBox = new QGroupBox(tr("Moving DICOM Source"));
  QVBoxLayout* loadLayout = new QVBoxLayout();
  QPushButton* loadButton = new QPushButton(tr("Load Source"));

  // Info Panel
  QGridLayout* details = new QGridLayout();
  QLabel* nameLabel = new QLabel(tr("<b>Name:</b>"));
  QLabel* slicesLabel = new QLabel(tr("<b>Slices:</b>"));
  QLabel* optimizerLabel = new QLabel(tr("<b>Metric Value:</b>"));

  m_UIDLabel = new QLabel(tr("-"));
  m_slicesLabel = new QLabel(tr("-"));
  m_optimizerLabel = new QLabel(tr("-"));
  details->addWidget(nameLabel, 1, 0);
  details->addWidget(m_UIDLabel, 1, 1);
  details->addWidget(slicesLabel, 2, 0);
  details->addWidget(m_slicesLabel, 2, 1);
  details->addWidget(optimizerLabel, 3, 0);
  details->addWidget(m_optimizerLabel, 3, 1);

  this->m_runButton = new QPushButton(tr("Register"));
  m_runButton->setDisabled(true);

  loadLayout->addWidget(loadButton);
  loadLayout->addItem(details);
  loadingBox->setLayout(loadLayout);

  // Add block elements to the page.
  pageLayout->addWidget(loadingBox);
  pageLayout->addWidget(m_runButton);
  m_toolBox->setLayout(pageLayout);

  connect(loadButton, SIGNAL(clicked()), this, SLOT(LoadMovingImage()));
  connect(m_runButton, SIGNAL(clicked()), this, SLOT(Register()));
}

void Registrant::LoadMovingImage() {
  QDir dir = QFileDialog::getExistingDirectory(0, "Select Folder: ");
  
  // generate file names using metadata to order correctly
  m_namesGenerator->SetInputDirectory( dir.path().toStdString().c_str() );
  m_reader->SetImageIO( m_gdcmIO );
  m_reader->SetFileNames( m_namesGenerator->GetInputFileNames() );

  try {
    m_reader->Update();
  } catch (itk::ExceptionObject& e) {
    CLOG(INFO, "register") << "Failed to load DICOM file: ";
    return;
  }

  Reader::DictionaryRawPointer dictionary =
    (*(m_reader->GetMetaDataDictionaryArray()))[0];

  // Expose the study UID metadata value and show it in our interface label.
  std::string studyUID;
  itk::ExposeMetaData<std::string>(*dictionary, "0020|000d", studyUID);
  CLOG(INFO, "register") << studyUID;

  QString labelText = QString::fromStdString(studyUID);
  QFontMetrics metrics(m_UIDLabel->font());
  m_UIDLabel->setText(metrics.elidedText(labelText, Qt::ElideRight,
    m_UIDLabel->width()));

  m_movingCaster->SetInput(m_reader->GetOutput());
  m_runButton->setDisabled(false);
}

void Registrant::BuildContent() {
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

  // Create volume rendering objects.
  m_opacityMap = vtkPiecewiseFunction::New();
  m_opacityMap->AddSegment(-255.0, 1.0, 255.0, 1.0);

  m_colorMap = vtkColorTransferFunction::New();
  m_colorMap->AddRGBSegment(0.0, 1.0, 1.0, 1.0, 255.0, 1.0, 1.0, 1.0);

  m_property = vtkVolumeProperty::New();
  m_property->SetColor(m_colorMap);
  m_property->SetScalarOpacity(m_opacityMap);
  m_property->SetInterpolationTypeToLinear();

  m_mapper = vtkFixedPointVolumeRayCastMapper::New();
  m_mapper->SetBlendModeToMaximumIntensity();

  m_volume = vtkVolume::New();
  m_volume->SetMapper(m_mapper);
  m_volume->SetProperty(m_property);
  m_renderer->AddVolume(m_volume);
  m_renderer->ResetCamera();

  // Setup checkerboard image plane widget.
  vtkProperty* prop = vtkProperty::New();
  vtkCellPicker* picker = vtkCellPicker::New();
  m_display = vtkImagePlaneWidget::New();
  m_display->SetInteractor(m_interactor);
  m_display->SetPicker(picker);
  m_display->RestrictPlaneToVolumeOn();
  m_display->SetTexturePlaneProperty(prop);
  m_display->TextureInterpolateOff();
  m_display->SetResliceInterpolateToLinear();
  m_display->SetInputData(m_converter->GetOutput());
  m_display->SetPlaneOrientationToXAxes();
  m_display->SetSliceIndex(0);
  m_display->DisplayTextOn();
  m_display->SetDefaultRenderer(m_renderer);
  m_display->SetWindowLevel(1358, -27);
  m_display->On();
  m_display->InteractionOn();

  m_renderer->GradientBackgroundOn();
  m_renderer->SetBackground(0.7, 0.7, 0.7);
  m_renderer->SetBackground2(0.2, 0.2, 0.2);
  m_renderer->Render();
}

void Registrant::Register() {
  m_runButton->setDisabled(true);
  m_runButton->repaint();

  m_optimizerLabel->setText(tr("Starting"));
  m_optimizerLabel->repaint();

  typedef Optimizer::ParametersType ParametersType;
  typedef itk::CenteredTransformInitializer< 
    Transform, InternalImage, InternalImage
  > TransformInitializer;

  TransformInitializer::Pointer initializer = TransformInitializer::New();
  initializer->SetTransform(m_transform);
  initializer->SetFixedImage(m_fixedCaster->GetOutput());
  initializer->SetMovingImage(m_movingCaster->GetOutput());
  initializer->MomentsOn();

  initializer->InitializeTransform();

  typedef Transform::VersorType Versor;
  typedef Versor::VectorType Vector;

  Vector axis;
  axis[0] = 0.0;
  axis[1] = 0.0;
  axis[2] = 1.0;

  Versor rotation;
  rotation.Set(axis, 0);
  
  m_transform->SetRotation(rotation);

  m_fixedCaster->Update();
  m_registration->SetFixedImageRegion(m_fixedCaster->GetOutput()->GetBufferedRegion());
  m_registration->SetInitialTransformParameters(m_transform->GetParameters());

  // Optimizer configuration.
  typedef Optimizer::ScalesType Scales;
  Scales scales(m_transform->GetNumberOfParameters());
  
  double data[6] = { 1/50.0,  1/50.0,  1/50.0, 10.0, 10.0, 10.0 };
  scales.SetData(data, 6, false);
  m_optimizer->SetScales(scales);

  // Metric and optimizer configuration.
  m_metric->SetNumberOfHistogramBins(m_histogramBins);
  m_metric->SetNumberOfSpatialSamples(m_histogramSamples);
  m_metric->ReinitializeSeed(76926294);
  m_optimizer->SetNumberOfIterations(m_iterations);

  // Set up observer callbacks to inspect the registration.
  RegistrationIterationCommand::Pointer observer = 
    RegistrationIterationCommand::New();
  observer->label = m_optimizerLabel;

  typedef RegistrationInterfaceCommand<Registration> InterfaceCommand;
  InterfaceCommand::Pointer interfaceCallback = InterfaceCommand::New();

  m_optimizer->AddObserver(itk::IterationEvent(), observer);
  m_registration->AddObserver(itk::IterationEvent(), interfaceCallback);

  m_registration->SetNumberOfLevels(m_levels);

  // Kick off registration.
  CLOG(INFO, "register") << "Starting registration ...";
  CLOG(INFO, "register")  << "Optimizer stop condition: "
    << m_registration->GetOptimizer()->GetStopConditionDescription();

  try {
    m_registration->Update();
  } catch (itk::ExceptionObject & err) {
    CLOG(INFO, "register") << "Registration error: ";
    CLOG(INFO, "register") << err.what();
  }

  // Get final solution parameters and unpack them.
  ParametersType finalParameters = m_registration->GetLastTransformParameters();

  double transX = finalParameters[3];
  double transY = finalParameters[4];
  double transZ = finalParameters[5];

  unsigned int iterations = m_optimizer->GetCurrentIteration();
  double bestValue = m_optimizer->GetValue();

  CLOG(INFO, "register") << "Result = " 
    << "( " << finalParameters[0] << ", " 
    << finalParameters[1] << ", " 
    << finalParameters[2] << " )";
  CLOG(INFO, "register") << "tX = " << transX;
  CLOG(INFO, "register") << "tY = " << transY;
  CLOG(INFO, "register") << "tZ = " << transZ;
  CLOG(INFO, "register") << "Iterations = " << iterations;
  CLOG(INFO, "register") << "Metric value = " << bestValue;

  m_optimizerLabel->setText(tr("Finalizing"));
  m_optimizerLabel->repaint();

  m_finalTransform->SetCenter(m_transform->GetCenter());
  m_finalTransform->SetParameters(finalParameters);
  m_finalTransform->SetFixedParameters(m_transform->GetFixedParameters());
  m_resample->SetTransform(m_finalTransform);
  m_resample->SetInput(m_reader->GetOutput());

  m_resample->SetSize(m_fixedImage->GetLargestPossibleRegion().GetSize());
  m_resample->SetOutputOrigin(m_fixedImage->GetOrigin());
  m_resample->SetOutputSpacing(m_fixedImage->GetSpacing());
  m_resample->SetOutputDirection(m_fixedImage->GetDirection());
  m_resample->SetDefaultPixelValue(m_greyLevel);

  // rescale registered moving input to fixed input intensity
  m_histmatch->ThresholdAtMeanIntensityOn();
  m_histmatch->SetNumberOfHistogramLevels(m_histogramBins);
  m_histmatch->SetNumberOfMatchPoints(m_histogramSamples);
  m_histmatch->Update();

  // Registration is complete at this point. m_histmatch provides the valid
  // output.
  UpdateView();
  emit RegistrationComplete(m_histmatch->GetOutput());
  m_optimizerLabel->setText(tr("-"));
  m_optimizerLabel->repaint();
  m_runButton->setDisabled(false);
}

void Registrant::UpdateView() {
  m_checkerBoard->SetInput1(m_fixedImage);
  m_checkerBoard->SetInput2(m_histmatch->GetOutput());
  m_converter->Update();

  m_display->SetInputData(m_converter->GetOutput());
  m_display->SetSliceIndex((int)(m_converter->GetOutput()->GetDimensions()[0] / 2.0));

  m_display->UpdatePlacement();

  m_mapper->SetInputData(m_converter->GetOutput());
  m_mapper->Update();
  CLOG(INFO, "register") << m_mapper->GetMinimumImageSampleDistance();

  m_renderer->ResetCamera();
  m_view->GetRenderWindow()->Render();
}

void Registrant::SetFixedSource(BaseImage::Pointer fixed) {
  m_fixedImage = fixed;
  m_fixedCaster->SetInput(fixed);
  m_histmatch->SetReferenceImage(fixed);
}

QWidget* Registrant::GetToolbox() {
  return m_toolBox;
}

QWidget* Registrant::GetContent() {
  return m_view;
}
