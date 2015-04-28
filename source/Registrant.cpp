#include "Registrant.h"

using namespace vis;


// Monitors the progress of the optimizer and changes the parameters as it goes
// CLOGs optimizer progress
template <typename TRegistration>
class RegistrationInterfaceCommand : public itk::Command
{
public:
  typedef  RegistrationInterfaceCommand   Self;
  typedef  itk::Command                   Superclass;
  typedef  itk::SmartPointer<Self>        Pointer;
  itkNewMacro( Self );
protected:
  RegistrationInterfaceCommand() {};
public:
  typedef   TRegistration                              RegistrationType;
  typedef   RegistrationType *                         RegistrationPointer;
#if TRANSFORM == AFFINE_TRANSFORM || TRANSFORM == TRANSLATION_TRANSFORM
  typedef   itk::RegularStepGradientDescentOptimizer   OptimizerType;
#elif TRANSFORM == VERSOR_RIGID_TRANSFORM
  typedef itk::VersorRigid3DTransformOptimizer         OptimizerType;
#endif
  typedef   OptimizerType *                            OptimizerPointer;
  void Execute(itk::Object * object, const itk::EventObject & event) ITK_OVERRIDE
    {
    if( !(itk::IterationEvent().CheckEvent( &event )) )
      {
      return;
      }
    RegistrationPointer registration = static_cast<RegistrationPointer>( object );
    if(registration == ITK_NULLPTR)
      {
      return;
      }
    OptimizerPointer optimizer = static_cast< OptimizerPointer >(registration->GetModifiableOptimizer() );
    CLOG(INFO, "register") << "-------------------------------------" << std::endl;
    CLOG(INFO, "register") << "MultiResolution Level : "
              << registration->GetCurrentLevel()  << std::endl;
    CLOG(INFO, "register") << std::endl;
    if ( registration->GetCurrentLevel() == 0 )
      {
      optimizer->SetMaximumStepLength( INITIAL_MAX_STEP );
      optimizer->SetMinimumStepLength( INITIAL_MIN_STEP );
      }
    else
      {
      optimizer->SetMaximumStepLength( optimizer->GetMaximumStepLength() / MAX_STEP_REDUCTION_FACTOR );
      optimizer->SetMinimumStepLength( optimizer->GetMinimumStepLength() / MIN_STEP_REDUCTION_FACTOR );
      }
    }
  void Execute(const itk::Object * , const itk::EventObject & ) ITK_OVERRIDE
    { return; }
};
// The following section of code implements an observer
// that will monitor the evolution of the registration process.
// CLOGs changes in the transfform
class CommandIterationUpdate : public itk::Command
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef  itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
protected:
  CommandIterationUpdate() {};
public:
#if TRANSFORM == AFFINE_TRANSFORM || TRANSFORM == TRANSLATION_TRANSFORM
  typedef   itk::RegularStepGradientDescentOptimizer   OptimizerType;
#elif TRANSFORM == VERSOR_RIGID_TRANSFORM
  typedef itk::VersorRigid3DTransformOptimizer         OptimizerType;
#endif
  typedef   const OptimizerType *                    OptimizerPointer;
  void Execute(itk::Object *caller, const itk::EventObject & event) ITK_OVERRIDE
    {
      Execute( (const itk::Object *)caller, event);
    }
  void Execute(const itk::Object * object, const itk::EventObject & event) ITK_OVERRIDE
    {
      OptimizerPointer optimizer = static_cast< OptimizerPointer >( object );
      if( !(itk::IterationEvent().CheckEvent( &event )) )
        {
        return;
        }
      CLOG(INFO, "register") << optimizer->GetCurrentIteration() << "   ";
      CLOG(INFO, "register") << optimizer->GetValue() << "   ";
      CLOG(INFO, "register") << optimizer->GetCurrentPosition() << std::endl;
    }
};



Registrant::Registrant() : Stage() {
  m_reader = Reader::New();
  m_converter = Converter::New();
  m_converter->SetInput(m_reader->GetOutput());

  //
  // initialize all the Registration components
  //
  transform     = TransformType::New();
  optimizer     = OptimizerType::New();
  interpolator  = InterpolatorType::New();
  registration  = RegistrationType::New();
  metric        = MetricType::New();
  fixedImagePyramid = FixedImagePyramidType::New();
  movingImagePyramid = MovingImagePyramidType::New();
  registration->SetOptimizer(     optimizer     );
  registration->SetTransform(     transform     );
  registration->SetInterpolator(  interpolator  );
  registration->SetMetric( metric  );
  registration->SetFixedImagePyramid( fixedImagePyramid );
  registration->SetMovingImagePyramid( movingImagePyramid );

  //
  // cast input imagetype to internal imagetype
  //
  fixedCaster  = FixedCastFilterType::New();
  movingCaster = MovingCastFilterType::New();
  movingCaster->SetInput( m_reader->GetOutput() );
  registration->SetFixedImage(    fixedCaster->GetOutput()    );
  registration->SetMovingImage(   movingCaster->GetOutput()   );
  // Create filter objects used later
  finalTransform = TransformType::New();
  resample = ResampleFilterType::New();
  caster = CastFilterType::New();
  checker = CheckerBoardFilterType::New();


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

  m_UIDLabel = new QLabel(tr("-"));
  m_slicesLabel = new QLabel(tr("-"));
  details->addWidget(nameLabel, 1, 0);
  details->addWidget(m_UIDLabel, 1, 1);
  details->addWidget(slicesLabel, 2, 0);
  details->addWidget(m_slicesLabel, 2, 1);

  loadLayout->addWidget(loadButton);
  loadLayout->addItem(details);
  loadingBox->setLayout(loadLayout);

  // Add block elements to the page.
  pageLayout->addWidget(loadingBox);
  m_toolBox->setLayout(pageLayout);

  connect(loadButton, SIGNAL(clicked()), this, SLOT(LoadMovingImage()));
}

void Registrant::LoadMovingImage() {
  QDir dir = QFileDialog::getExistingDirectory(0, "Select Folder: ");
  QFileInfoList list = dir.entryInfoList(QDir::Dirs | QDir::Files | 
      QDir::NoDotAndDotDot);

  std::vector<std::string> names;
  foreach (QFileInfo finfo, list) {
    std::string str = dir.path().toStdString().c_str();
    str.append("/");

    names.push_back(str + finfo.fileName().toStdString().c_str());
  }
  m_reader->SetFileNames(names);

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

  // m_reader is valid here, tell the visualize/registration function?
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

  m_renderer->GradientBackgroundOn();
  m_renderer->SetBackground(0.7, 0.7, 0.7);
  m_renderer->SetBackground2(0.2, 0.2, 0.2);
  m_renderer->Render();
}

void Registrant::Update() {
  //
  // Code directly tied to Update call on registration object, creates objects on stack
  //

  // setup initial parameters
#if TRANSFORM == VERSOR_RIGID_TRANSFORM
  typedef OptimizerType::ParametersType ParametersType;
  typedef itk::CenteredTransformInitializer< TransformType, 
                                             InternalImageType, 
                                             InternalImageType 
                                                 >  TransformInitializerType;
  TransformInitializerType::Pointer initializer = 
                                           TransformInitializerType::New();
  initializer->SetTransform(   transform );
  initializer->SetFixedImage(  fixedCaster->GetOutput() );
  initializer->SetMovingImage( movingCaster->GetOutput() );
  initializer->MomentsOn();

  initializer->InitializeTransform();

  typedef TransformType::VersorType  VersorType;
  typedef VersorType::VectorType     VectorType;
  VersorType     rotation;
  VectorType     axis;
  axis[0] = 0.0;
  axis[1] = 0.0;
  axis[2] = 1.0;
  const double angle = 0;
  rotation.Set(  axis, angle  );
  transform->SetRotation( rotation );

  registration->SetInitialTransformParameters( transform->GetParameters() );
  typedef OptimizerType::ScalesType       OptimizerScalesType;
  OptimizerScalesType optimizerScales( transform->GetNumberOfParameters() );
  const double translationScale = 1.0 / 1000.0;
  optimizerScales[0] = 1.0;
  optimizerScales[1] = 1.0;
  optimizerScales[2] = 1.0;
  optimizerScales[3] = translationScale;
  optimizerScales[4] = translationScale;
  optimizerScales[5] = translationScale;
  optimizer->SetScales( optimizerScales );

#elif TRANSFORM == AFFINE_TRANSFORM || TRANSFORM == TRANSLATION_TRANSFORM
  typedef RegistrationType::ParametersType ParametersType;
  ParametersType initialParameters( transform->GetNumberOfParameters() );
#if TRANSFORM == AFFINE_TRANSFORM
  initialParameters[0] = 1.0;
  initialParameters[1] = 0.0;
  initialParameters[2] = 0.0;
  initialParameters[3] = 0.0;
  initialParameters[4] = 1.0;
  initialParameters[5] = 0.0;
  initialParameters[6] = 0.0;
  initialParameters[7] = 0.0;
  initialParameters[8] = 1.0;
  initialParameters[9] = 0.0;  // Initial offset in mm along X
  initialParameters[10] = 0.0;  // Initial offset in mm along Y
  initialParameters[11] = 0.0;  // Initial offset in mm along Z
#elif TRANSFORM == TRANSLATION_TRANSFORM
  initialParameters[0] = 0.0;  // Initial offset in mm along X
  initialParameters[1] = 0.0;  // Initial offset in mm along Y
  initialParameters[2] = 0.0;  // Initial offset in mm along Z
#endif
  registration->SetInitialTransformParameters( initialParameters );
#endif

  // Set metric parameters
  //
  metric->SetNumberOfHistogramBins( NUMBER_HISTOGRAM_BINS );
  metric->SetNumberOfSpatialSamples( NUMBER_HISTOGRAM_SAMPLES );
  metric->ReinitializeSeed( 76926294 );
  optimizer->SetNumberOfIterations( NUMBER_ITERATIONS );
#if TRANSFORM == AFFINE_TRANSFORM || TRANSFORM == TRANSLATION_TRANSFORM
  optimizer->SetRelaxationFactor( RELAXATION_FACTOR );
#endif


  // Create the Command observer and register it with the optimizer.
  //
  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );
  typedef RegistrationInterfaceCommand<RegistrationType> CommandType;
  CommandType::Pointer command = CommandType::New();
  registration->AddObserver( itk::IterationEvent(), command );


  registration->SetNumberOfLevels( REGISTRATION_NUMBER_LEVELS );
  //
  // Do actual Registration
  //
  try
  {
    CLOG(INFO, "register") << "Starting registration ..." << std::endl;
    registration->Update();
    CLOG(INFO, "register")  << "Optimizer stop condition: "
              << registration->GetOptimizer()->GetStopConditionDescription()
              << std::endl;
  }
  catch( itk::ExceptionObject & err )
  {
    CLOG(INFO, "register") << "ExceptionObject caught !" << std::endl;
    CLOG(INFO, "register") << err.what() << std::endl;
    // return EXIT_FAILURE;
  }


  //
  // Get Final Parameters, parameters objects on stack
  //
  ParametersType finalParameters = registration->GetLastTransformParameters();
#if TRANSFORM == AFFINE_TRANSFORM
  double TranslationAlongX = finalParameters[9];
  double TranslationAlongY = finalParameters[10];
  double TranslationAlongZ = finalParameters[11];
#elif TRANSFORM == TRANSLATION_TRANSFORM
  double TranslationAlongX = finalParameters[0];
  double TranslationAlongY = finalParameters[1];
  double TranslationAlongZ = finalParameters[2];
#elif TRANSFORM == VERSOR_RIGID_TRANSFORM
  double TranslationAlongX = finalParameters[3];
  double TranslationAlongY = finalParameters[4];
  double TranslationAlongZ = finalParameters[5];
#endif
  unsigned int numberOfIterations = optimizer->GetCurrentIteration();
  double bestValue = optimizer->GetValue();

 // CLOG final parameters
  CLOG(INFO, "register") << "Result = " << std::endl;
#if TRANSFORM == AFFINE_TRANSFORM
  CLOG(INFO, "register") << "| " << finalParameters[0] << " " << finalParameters[1] << " " << finalParameters[2] << " |" << std::endl;
  CLOG(INFO, "register") << "| " << finalParameters[3] << " " << finalParameters[4] << " " << finalParameters[5] << " |" << std::endl;
  CLOG(INFO, "register") << "| " << finalParameters[6] << " " << finalParameters[7] << " " << finalParameters[8] << " |" << std::endl;
#elif TRANSFORM == VERSOR_RIGID_TRANSFORM
  CLOG(INFO, "register") << "( " << finalParameters[0] << " " << finalParameters[1] << " " << finalParameters[2] << " )" << std::endl;
#endif
  CLOG(INFO, "register") << " Translation X = " << TranslationAlongX  << std::endl;
  CLOG(INFO, "register") << " Translation Y = " << TranslationAlongY  << std::endl;
  CLOG(INFO, "register") << " Translation Z = " << TranslationAlongZ  << std::endl;
  CLOG(INFO, "register") << " Iterations    = " << numberOfIterations << std::endl;
  CLOG(INFO, "register") << " Metric value  = " << bestValue          << std::endl;


  //
  // generate final images
  //
#if TRANSFORM == VERSOR_RIGID_TRANSFORM
  finalTransform->SetCenter( transform->GetCenter() );
#endif
  finalTransform->SetParameters( finalParameters );
  finalTransform->SetFixedParameters( transform->GetFixedParameters() );
  resample->SetTransform( finalTransform );
  resample->SetInput( m_reader->GetOutput() );

  // resample to get final output image
  resample->SetSize(    fixedImage->GetLargestPossibleRegion().GetSize() );
  resample->SetOutputOrigin(  fixedImage->GetOrigin() );
  resample->SetOutputSpacing( fixedImage->GetSpacing() );
  resample->SetOutputDirection( fixedImage->GetDirection() );
  resample->SetDefaultPixelValue( DEFAULT_GRAY_LEVEL );
  caster->SetInput( resample->GetOutput() );
  // final image output
  caster->Update();
  finalImage = caster->GetOutput();


  //
  // Generate checkerboards before and after registration
  //
  checker->SetInput1( fixedImage );
  checker->SetInput2( resample->GetOutput() );
  caster->SetInput( checker->GetOutput() );
  resample->SetDefaultPixelValue( 0 );

  // Before registration checkerboard
  TransformType::Pointer identityTransform = TransformType::New();
  identityTransform->SetIdentity();
  resample->SetTransform( identityTransform );
  caster->Update();
  beforeRegisterCheckerImage = caster->GetOutput();

  // After registration checkerboard
  resample->SetTransform( finalTransform );
  caster->Update();
  afterRegisterCheckerImage = caster->GetOutput();

}

void Registrant::UpdateView() {}

void Registrant::SetFixedSource(BaseImage::Pointer fixed) {
  fixedImage = fixed;
  fixedCaster->SetInput(  fixedImage );
  fixedCaster->Update();
  registration->SetFixedImageRegion( fixedCaster->GetOutput()->GetBufferedRegion() );
  Update();
}

QWidget* Registrant::GetToolbox() {
  return m_toolBox;
}

QWidget* Registrant::GetContent() {
  return m_view;
}
