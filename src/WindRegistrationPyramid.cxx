/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
//
// This example is the 3D version of the 2D example in MultiResImageRegistration1.cxx
//
// modified for group Wind CSC 821/621 Spring 15 SFSU
//

#define TRANSLATION_TRANSFORM 1
#define AFFINE_TRANSFORM 2
#define VERSOR_RIGID_TRANSFORM 3

#define TRANSFORM VERSOR_RIGID_TRANSFORM

#include "itkMultiResolutionImageRegistrationMethod.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkRegularStepGradientDescentOptimizer.h"
#if TRANSFORM == AFFINE_TRANSFORM
#include "itkAffineTransform.h"
#elif TRANSFORM == TRANSLATION_TRANSFORM
#include "itkTranslationTransform.h"
#elif TRANSFORM == VERSOR_RIGID_TRANSFORM
#include "itkVersorRigid3DTransform.h"
#include "itkCenteredTransformInitializer.h"
#include "itkVersorRigid3DTransformOptimizer.h"
#else
uh-oh;
#endif
#include "itkImage.h"
// #include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageSeriesReader.h"
// #include "itkImage.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkCheckerBoardImageFilter.h"
#include "itkCommand.h"

#define INITIAL_MAX_STEP 0.2
#define INITIAL_MIN_STEP 0.0001
#define MAX_STEP_REDUCTION_FACTOR 1.0
#define MIN_STEP_REDUCTION_FACTOR 1.0

#define REGISTRATION_NUMBER_LEVELS 3
#define NUMBER_HISTOGRAM_BINS 128
#define NUMBER_HISTOGRAM_SAMPLES 50000
#define NUMBER_ITERATIONS  200
#define RELAXATION_FACTOR  0.9
#define DEFAULT_GRAY_LEVEL 0

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
    std::cout << "-------------------------------------" << std::endl;
    std::cout << "MultiResolution Level : "
              << registration->GetCurrentLevel()  << std::endl;
    std::cout << std::endl;
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
//  The following section of code implements an observer
//  that will monitor the evolution of the registration process.
//
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
      std::cout << optimizer->GetCurrentIteration() << "   ";
      std::cout << optimizer->GetValue() << "   ";
      std::cout << optimizer->GetCurrentPosition() << std::endl;
    }
};

int main( int argc, char *argv[] )
{
  if( argc < 3 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " fixedImageFile  movingImageFile ";
    std::cerr << " [outputImagefile] [backgroundGrayLevel]";
    std::cerr << " [checkerBoardBefore] [checkerBoardAfter]";
    std::cerr << " [useExplicitPDFderivatives ] " << std::endl;
    std::cerr << " [numberOfBins] [numberOfSamples ] " << std::endl;
    return EXIT_FAILURE;
    }
  const    unsigned int    Dimension = 3;
  typedef  unsigned short  PixelType;
  typedef itk::Image< PixelType, Dimension >  FixedImageType;
  typedef itk::Image< PixelType, Dimension >  MovingImageType;
  typedef   float                                    InternalPixelType;
  typedef itk::Image< InternalPixelType, Dimension > InternalImageType;
  
#if TRANSFORM == AFFINE_TRANSFORM
  typedef itk::AffineTransform< double, Dimension > TransformType;
#elif TRANSFORM == TRANSLATION_TRANSFORM
  typedef itk::TranslationTransform< double, Dimension > TransformType;
#elif TRANSFORM == VERSOR_RIGID_TRANSFORM
  typedef itk::VersorRigid3DTransform< double > TransformType;
#endif
#if TRANSFORM == AFFINE_TRANSFORM || TRANSFORM == TRANSLATION_TRANSFORM
  typedef   itk::RegularStepGradientDescentOptimizer   OptimizerType;
#elif TRANSFORM == VERSOR_RIGID_TRANSFORM
  typedef itk::VersorRigid3DTransformOptimizer         OptimizerType;
#endif
  typedef itk::LinearInterpolateImageFunction<
                                    InternalImageType,
                                    double             > InterpolatorType;
  typedef itk::MattesMutualInformationImageToImageMetric<
                                    InternalImageType,
                                    InternalImageType >   MetricType;
  typedef itk::MultiResolutionImageRegistrationMethod<
                                    InternalImageType,
                                    InternalImageType >   RegistrationType;
  typedef itk::MultiResolutionPyramidImageFilter<
                                    InternalImageType,
                                    InternalImageType >   FixedImagePyramidType;
  typedef itk::MultiResolutionPyramidImageFilter<
                                    InternalImageType,
                                    InternalImageType >   MovingImagePyramidType;
  //  All the components are instantiated using their \code{New()} method
  //  and connected to the registration object as in previous example.
  //
  TransformType::Pointer      transform     = TransformType::New();
  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
  RegistrationType::Pointer   registration  = RegistrationType::New();
  MetricType::Pointer         metric        = MetricType::New();
  FixedImagePyramidType::Pointer fixedImagePyramid =
      FixedImagePyramidType::New();
  MovingImagePyramidType::Pointer movingImagePyramid =
      MovingImagePyramidType::New();
  registration->SetOptimizer(     optimizer     );
  registration->SetTransform(     transform     );
  registration->SetInterpolator(  interpolator  );
  registration->SetMetric( metric  );
  registration->SetFixedImagePyramid( fixedImagePyramid );
  registration->SetMovingImagePyramid( movingImagePyramid );

  // typedef itk::ImageFileReader< FixedImageType  > FixedImageReaderType;
  // typedef itk::ImageFileReader< MovingImageType > MovingImageReaderType;
  // FixedImageReaderType::Pointer  fixedImageReader  = FixedImageReaderType::New();
  // MovingImageReaderType::Pointer movingImageReader = MovingImageReaderType::New();
  // fixedImageReader->SetFileName(  argv[1] );
  // movingImageReader->SetFileName( argv[2] );

  typedef itk::ImageSeriesReader< FixedImageType  >   FixedDicomReaderType;
  typedef itk::ImageSeriesReader< MovingImageType >   MovingDicomReaderType;
  FixedDicomReaderType::Pointer   fixedImageReader     = FixedDicomReaderType::New();
  MovingDicomReaderType::Pointer  movingImageReader    = MovingDicomReaderType::New();

  typedef itk::GDCMImageIO       FixedImageIOType;
  typedef itk::GDCMImageIO       MovingImageIOType;
  FixedImageIOType::Pointer fixedDicomIO = FixedImageIOType::New();
  MovingImageIOType::Pointer movingDicomIO = MovingImageIOType::New();

  fixedImageReader->SetImageIO( fixedDicomIO );
  movingImageReader->SetImageIO( movingDicomIO );
  
  //fixed
  typedef itk::GDCMSeriesFileNames FixedNamesGeneratorType;
  FixedNamesGeneratorType::Pointer fixedNameGenerator = FixedNamesGeneratorType::New();
  fixedNameGenerator->SetUseSeriesDetails( true );
  //nameGenerator->AddSeriesRestriction("0008|0021" );
  fixedNameGenerator->SetDirectory( argv[1] );
  
  //moving
  typedef itk::GDCMSeriesFileNames MovingNamesGeneratorType;
  MovingNamesGeneratorType::Pointer movingNameGenerator = MovingNamesGeneratorType::New();
  movingNameGenerator->SetUseSeriesDetails( true );
  //nameGenerator->AddSeriesRestriction("0008|0021" );
  movingNameGenerator->SetDirectory( argv[2] );
  
  //fixed  
  typedef std::vector< std::string >    FixedSeriesIdContainer;
  const FixedSeriesIdContainer & fixedSeriesUID = fixedNameGenerator->GetSeriesUIDs();
  FixedSeriesIdContainer::const_iterator fixedSeriesItr = fixedSeriesUID.begin();
  FixedSeriesIdContainer::const_iterator fixedSeriesEnd = fixedSeriesUID.end();
  while( fixedSeriesItr != fixedSeriesEnd )
  {
  std::cout << fixedSeriesItr->c_str() << std::endl;
  ++fixedSeriesItr;
  }
  std::string fixedSeriesIdentifier;
  fixedSeriesIdentifier = fixedSeriesUID.begin()->c_str();
  typedef std::vector< std::string >   FixedFileNamesContainer;
  FixedFileNamesContainer fixedFileNames;
  fixedFileNames = fixedNameGenerator->GetFileNames( fixedSeriesIdentifier );
  fixedImageReader->SetFileNames( fixedFileNames );
  try
  {
  fixedImageReader->Update();
  } 
  catch (itk::ExceptionObject &ex)
  {
  std::cout << ex << std::endl;
  return EXIT_FAILURE;
  }
  
  //moving
  typedef std::vector< std::string >    MovingSeriesIdContainer;
  const MovingSeriesIdContainer & movingSeriesUID = movingNameGenerator->GetSeriesUIDs();
  MovingSeriesIdContainer::const_iterator movingSeriesItr = movingSeriesUID.begin();
  MovingSeriesIdContainer::const_iterator movingSeriesEnd = movingSeriesUID.end();
  while( movingSeriesItr != movingSeriesEnd )
  {
  std::cout << movingSeriesItr->c_str() << std::endl;
  ++movingSeriesItr;
  }
  std::string movingSeriesIdentifier;
  movingSeriesIdentifier = movingSeriesUID.begin()->c_str();
  typedef std::vector< std::string >   MovingFileNamesContainer;
  MovingFileNamesContainer movingFileNames;
  movingFileNames = movingNameGenerator->GetFileNames( movingSeriesIdentifier );
  movingImageReader->SetFileNames( movingFileNames );
  try
  {
  movingImageReader->Update();
  } 
  catch (itk::ExceptionObject &ex)
  {
  std::cout << ex << std::endl;
  return EXIT_FAILURE;
  }

  typedef itk::CastImageFilter<
                        FixedImageType, InternalImageType > FixedCastFilterType;
  typedef itk::CastImageFilter<
                        MovingImageType, InternalImageType > MovingCastFilterType;
  FixedCastFilterType::Pointer fixedCaster   = FixedCastFilterType::New();
  MovingCastFilterType::Pointer movingCaster = MovingCastFilterType::New();
  fixedCaster->SetInput(  fixedImageReader->GetOutput() );
  movingCaster->SetInput( movingImageReader->GetOutput() );
  registration->SetFixedImage(    fixedCaster->GetOutput()    );
  registration->SetMovingImage(   movingCaster->GetOutput()   );
  fixedCaster->Update();
  registration->SetFixedImageRegion(
       fixedCaster->GetOutput()->GetBufferedRegion() );

#if TRANSFORM == VERSOR_RIGID_TRANSFORM
  typedef OptimizerType::ParametersType ParametersType;
  typedef itk::CenteredTransformInitializer< TransformType, 
                                             FixedImageType, 
                                             MovingImageType 
                                                 >  TransformInitializerType;
  TransformInitializerType::Pointer initializer = 
                                           TransformInitializerType::New();
  initializer->SetTransform(   transform );
  initializer->SetFixedImage(  fixedImageReader->GetOutput() );
  initializer->SetMovingImage( movingImageReader->GetOutput() );
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

  metric->SetNumberOfHistogramBins( NUMBER_HISTOGRAM_BINS );
  metric->SetNumberOfSpatialSamples( NUMBER_HISTOGRAM_SAMPLES );
  if( argc > 8 )
    {
    // optionally, override the values with numbers taken from the command line arguments.
    metric->SetNumberOfHistogramBins( atoi( argv[8] ) );
    }
  if( argc > 9 )
    {
    // optionally, override the values with numbers taken from the command line arguments.
    metric->SetNumberOfSpatialSamples( atoi( argv[9] ) );
    }
  metric->ReinitializeSeed( 76926294 );
  if( argc > 7 )
    {
    // Define whether to calculate the metric derivative by explicitly
    // computing the derivatives of the joint PDF with respect to the Transform
    // parameters, or doing it by progressively accumulating contributions from
    // each bin in the joint PDF.
    metric->SetUseExplicitPDFDerivatives( atoi( argv[7] ) );
    }
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
  try
    {
      std::cout << "Starting registration ..." << std::endl;
      registration->Update();
      std::cout << "Optimizer stop condition: "
                << registration->GetOptimizer()->GetStopConditionDescription()
                << std::endl;
    }
  catch( itk::ExceptionObject & err )
    {
    std::cout << "ExceptionObject caught !" << std::endl;
    std::cout << err << std::endl;
    return EXIT_FAILURE;
    }

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
  // Print out results
  //
  std::cout << "Result = " << std::endl;
#if TRANSFORM == AFFINE_TRANSFORM
  std::cout << "| " << finalParameters[0] << " " << finalParameters[1] << " " << finalParameters[2] << " |" << std::endl;
  std::cout << "| " << finalParameters[3] << " " << finalParameters[4] << " " << finalParameters[5] << " |" << std::endl;
  std::cout << "| " << finalParameters[6] << " " << finalParameters[7] << " " << finalParameters[8] << " |" << std::endl;
#elif TRANSFORM == VERSOR_RIGID_TRANSFORM
  std::cout << "( " << finalParameters[0] << " " << finalParameters[1] << " " << finalParameters[2] << " )" << std::endl;
#endif
  std::cout << " Translation X = " << TranslationAlongX  << std::endl;
  std::cout << " Translation Y = " << TranslationAlongY  << std::endl;
  std::cout << " Translation Z = " << TranslationAlongZ  << std::endl;
  std::cout << " Iterations    = " << numberOfIterations << std::endl;
  std::cout << " Metric value  = " << bestValue          << std::endl;

  typedef itk::ResampleImageFilter<
                            MovingImageType,
                            FixedImageType >    ResampleFilterType;
  TransformType::Pointer finalTransform = TransformType::New();
#if TRANSFORM == VERSOR_RIGID_TRANSFORM
  finalTransform->SetCenter( transform->GetCenter() );
#endif
  finalTransform->SetParameters( finalParameters );
  finalTransform->SetFixedParameters( transform->GetFixedParameters() );
  ResampleFilterType::Pointer resample = ResampleFilterType::New();
  resample->SetTransform( finalTransform );
  resample->SetInput( movingImageReader->GetOutput() );
  FixedImageType::Pointer fixedImage = fixedImageReader->GetOutput();
  PixelType backgroundGrayLevel = DEFAULT_GRAY_LEVEL;
  if( argc > 4 )
    {
    backgroundGrayLevel = atoi( argv[4] );
    }
  resample->SetSize(    fixedImage->GetLargestPossibleRegion().GetSize() );
  resample->SetOutputOrigin(  fixedImage->GetOrigin() );
  resample->SetOutputSpacing( fixedImage->GetSpacing() );
  resample->SetOutputDirection( fixedImage->GetDirection() );
  resample->SetDefaultPixelValue( backgroundGrayLevel );
  typedef  unsigned char  OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension > OutputImageType;
  typedef itk::CastImageFilter<
                        FixedImageType,
                        OutputImageType > CastFilterType;
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;
  WriterType::Pointer      writer =  WriterType::New();
  CastFilterType::Pointer  caster =  CastFilterType::New();
  writer->SetFileName( "outputPyramid.dcm" );
  caster->SetInput( resample->GetOutput() );
  writer->SetInput( caster->GetOutput()   );
  writer->Update();

  //
  // Generate checkerboards before and after registration
  //
  typedef itk::CheckerBoardImageFilter< FixedImageType > CheckerBoardFilterType;
  CheckerBoardFilterType::Pointer checker = CheckerBoardFilterType::New();
  checker->SetInput1( fixedImage );
  checker->SetInput2( resample->GetOutput() );
  caster->SetInput( checker->GetOutput() );
  writer->SetInput( caster->GetOutput()   );
  resample->SetDefaultPixelValue( 0 );
  // Before registration
  TransformType::Pointer identityTransform = TransformType::New();
  identityTransform->SetIdentity();
  resample->SetTransform( identityTransform );
  writer->SetFileName( "beforePyramidChecker.dcm" );
  writer->Update();

  // After registration
  resample->SetTransform( finalTransform );
  writer->SetFileName( "afterPyramidChecker.dcm" );
  writer->Update();

  return EXIT_SUCCESS;
}
