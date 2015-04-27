
#define MUTUAL_INFO 1

#include "itkImageRegistrationMethod.h"
#include "itkAffineTransform.h"
#if MUTUAL_INFO == 1
#include "itkMutualInformationImageToImageMetric.h"
#else
#include "itkMeanSquaresImageToImageMetric.h"
#endif
#include "itkGradientDescentOptimizer.h"
#include "itkNormalizeImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkCheckerBoardImageFilter.h"
#include "itkEllipseSpatialObject.h"
#include "itkSpatialObjectToImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkImageSeriesReader.h"
#include "itkImage.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkShrinkImageFilter.h"
#include <ctime>

#define SMOOTHER_VARIANCE 2.0
#define SHRINK_FACTOR 2
#define METRIC_STANDARD_DEVIANTION 0.4
#define SAMPLE_FRACTION 0.025
#define LEARNING_RATE 0.01
#define MAX_ITERATIONS 100
#define DEFAULT_PIXEL_VALUE 100


std::clock_t start_timer;


class CommandIterationUpdate : public itk::Command
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;

  typedef itk::SmartPointer<Self>  Pointer;
  
  itkNewMacro( Self );
  

protected:
  CommandIterationUpdate() {};

public:
  
  typedef itk::GradientDescentOptimizer OptimizerType;
  typedef const OptimizerType *                              OptimizerPointer;
  
  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    Execute( (const itk::Object *)caller, event);
    }
  
  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
    
    OptimizerPointer optimizer =
                         static_cast< OptimizerPointer >( object );
    
    if( ! itk::IterationEvent().CheckEvent( &event ) )
      {
      return;
      }
    
    std::cout << optimizer->GetCurrentIteration() << " = ";
    std::cout << optimizer->GetValue() << " : " << ( std::clock() - start_timer ) / (double) CLOCKS_PER_SEC << "s" << std::endl;
    // std::cout << optimizer->GetCurrentPosition() << std::endl;

    start_timer = std::clock();
   
    }

};
 
int main( int argc, char *argv[] )
{  
  if( argc < 3 )
  {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " fixedDicomDirectory  movingDicomDirectory " << std::endl;
    return EXIT_FAILURE;
  }
  
  const    unsigned int    Dimension = 3;
  typedef  unsigned short   PixelType;

  typedef itk::Image< PixelType, Dimension >  ImageType;
  
  typedef itk::ImageSeriesReader< ImageType  >   FixedDicomReaderType;
  typedef itk::ImageSeriesReader< ImageType >   MovingDicomReaderType;
  FixedDicomReaderType::Pointer   fixedDicomReader     = FixedDicomReaderType::New();
  MovingDicomReaderType::Pointer  movingDicomReader    = MovingDicomReaderType::New();

  typedef itk::GDCMImageIO       FixedImageIOType;
  typedef itk::GDCMImageIO       MovingImageIOType;
  FixedImageIOType::Pointer fixedDicomIO = FixedImageIOType::New();
  MovingImageIOType::Pointer movingDicomIO = MovingImageIOType::New();

  fixedDicomReader->SetImageIO( fixedDicomIO );
  movingDicomReader->SetImageIO( movingDicomIO );
  
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
  
  fixedDicomReader->SetFileNames( fixedFileNames );

  try
  {
	fixedDicomReader->Update();
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
  
  movingDicomReader->SetFileNames( movingFileNames );
  
  try
  {
	movingDicomReader->Update();
  } 
  catch (itk::ExceptionObject &ex)
  {
	std::cout << ex << std::endl;
	return EXIT_FAILURE;
  }
  ///// At this point, we have a volumetric image in memory that we can access by
  ///// invoking the \code{GetOutput()} method of the reader.
  // std::cout << "We have two images now" << std::endl;

  // Images are unsigned char pixel type but use floats internally 
  typedef   float                       InternalPixelType;
  typedef itk::Image< float, Dimension> InternalImageType;
  
  // Normalize the images
  typedef itk::NormalizeImageFilter<ImageType, InternalImageType> NormalizeFilterType;
  
  NormalizeFilterType::Pointer fixedNormalizer = NormalizeFilterType::New();
  NormalizeFilterType::Pointer movingNormalizer = NormalizeFilterType::New();
  
  fixedNormalizer->SetInput(  fixedDicomReader->GetOutput());
  movingNormalizer->SetInput( movingDicomReader->GetOutput());
  
  // Smooth the images
  typedef itk::DiscreteGaussianImageFilter<InternalImageType,InternalImageType> GaussianFilterType;
 
  GaussianFilterType::Pointer fixedSmoother  = GaussianFilterType::New();
  GaussianFilterType::Pointer movingSmoother = GaussianFilterType::New();
 
  fixedSmoother->SetVariance( SMOOTHER_VARIANCE );
  movingSmoother->SetVariance( SMOOTHER_VARIANCE );
 
  fixedSmoother->SetInput( fixedNormalizer->GetOutput() );
  movingSmoother->SetInput( movingNormalizer->GetOutput() );
  
  // declare all registration components 
  typedef itk::AffineTransform< double, Dimension > TransformType;
  typedef itk::GradientDescentOptimizer                  OptimizerType;
  typedef itk::LinearInterpolateImageFunction<
                                    InternalImageType,
                                    double             > InterpolatorType;
  typedef itk::ImageRegistrationMethod<
                                    InternalImageType,
                                    InternalImageType >  RegistrationType;
#if MUTUAL_INFO == 1
  typedef itk::MutualInformationImageToImageMetric<
                                          InternalImageType,
                                          InternalImageType >    MetricType;
#else
  typedef itk::MeanSquaresImageToImageMetric< 
                                       InternalImageType, 
                                       InternalImageType >    MetricType;
#endif
 


 
  TransformType::Pointer      transform     = TransformType::New();
  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
  RegistrationType::Pointer   registration  = RegistrationType::New();
  
  // std::cout << "Registration objects created." << std::endl;
  
  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );
 
  registration->SetOptimizer(     optimizer     );
  registration->SetTransform(     transform     );
  registration->SetInterpolator(  interpolator  );
 
  MetricType::Pointer         metric        = MetricType::New();
  registration->SetMetric( metric  );
 
  //  The metric requires a number of parameters to be selected, including
  //  the standard deviation of the Gaussian kernel for the fixed image
  //  density estimate, the standard deviation of the kernel for the moving
  //  image density and the number of samples use to compute the densities
  //  and entropy values. Details on the concepts behind the computation of
  //  the metric can be found in Section
  //  \ref{sec:MutualInformationMetric}.  Experience has
  //  shown that a kernel standard deviation of $0.4$ works well for images
  //  which have been normalized to a mean of zero and unit variance.  We
  //  will follow this empirical rule in this example.
#if MUTUAL_INFO == 1
  metric->SetFixedImageStandardDeviation(  METRIC_STANDARD_DEVIANTION );
  metric->SetMovingImageStandardDeviation( METRIC_STANDARD_DEVIANTION );
#endif
  

  // shrink images to improve registration time
  typedef itk::ShrinkImageFilter <InternalImageType, InternalImageType>
          ShrinkImageFilterType;
 
  ShrinkImageFilterType::Pointer fixedShrinkFilter = ShrinkImageFilterType::New();
  ShrinkImageFilterType::Pointer movingShrinkFilter = ShrinkImageFilterType::New();

  fixedShrinkFilter->SetInput( fixedSmoother->GetOutput() );
  movingShrinkFilter->SetInput( movingSmoother->GetOutput() );

  fixedShrinkFilter->SetShrinkFactor(0, SHRINK_FACTOR);
  fixedShrinkFilter->SetShrinkFactor(1, SHRINK_FACTOR); 
  fixedShrinkFilter->SetShrinkFactor(3, SHRINK_FACTOR);
  movingShrinkFilter->SetShrinkFactor(0, SHRINK_FACTOR);
  movingShrinkFilter->SetShrinkFactor(1, SHRINK_FACTOR); 
  movingShrinkFilter->SetShrinkFactor(3, SHRINK_FACTOR);


  registration->SetFixedImage(    fixedShrinkFilter->GetOutput()    );
  registration->SetMovingImage(   movingShrinkFilter->GetOutput()   );


  fixedShrinkFilter->Update();
  ImageType::RegionType fixedImageRegion = fixedShrinkFilter->GetOutput()->GetBufferedRegion();//GetLargestPossibleRegion();
  registration->SetFixedImageRegion( fixedImageRegion );
 
  typedef RegistrationType::ParametersType ParametersType;
  ParametersType initialParameters( transform->GetNumberOfParameters() );
 
  // rotation matrix (identity)
  initialParameters[0] = 1.0;  // R(0,0)
  initialParameters[1] = 0.0;  // R(0,1)
  initialParameters[2] = 0.0;  // R(0,2)
  initialParameters[3] = 0.0;  // R(1,0)
  initialParameters[4] = 1.0;  // R(1,1)
  initialParameters[5] = 0.0;  // R(1,2)
  initialParameters[6] = 0.0;  // R(2,0)
  initialParameters[7] = 0.0;  // R(2,1)
  initialParameters[8] = 1.0;  // R(2,2)
 
  // translation vector
  initialParameters[9] = 0.0;
  initialParameters[10] = 0.0;
  initialParameters[11] = 0.0;
 
  registration->SetInitialTransformParameters( initialParameters );
 
  // std::cout << "parameters are set." << std::endl;
  
  //  Software Guide : BeginLatex
  //
  //  We should now define the number of spatial samples to be considered in
  //  the metric computation. Note that we were forced to postpone this setting
  //  until we had done the preprocessing of the images because the number of
  //  samples is usually defined as a fraction of the total number of pixels in
  //  the fixed image.
  //
  //  The number of spatial samples can usually be as low as $1\%$ of the total
  //  number of pixels in the fixed image. Increasing the number of samples
  //  improves the smoothness of the metric from one iteration to another and
  //  therefore helps when this metric is used in conjunction with optimizers
  //  that rely of the continuity of the metric values. The trade-off, of
  //  course, is that a larger number of samples result in longer computation
  //  times per every evaluation of the metric.
  //
  //  It has been demonstrated empirically that the number of samples is not a
  //  critical parameter for the registration process. When you start fine
  //  tuning your own registration process, you should start using high values
  //  of number of samples, for example in the range of $20\%$ to $50\%$ of the
  //  number of pixels in the fixed image. Once you have succeeded to register
  //  your images you can then reduce the number of samples progressively until
  //  you find a good compromise on the time it takes to compute one evaluation
  //  of the Metric. Note that it is not useful to have very fast evaluations
  //  of the Metric if the noise in their values results in more iterations
  //  being required by the optimizer to converge. You must then study the
  //  behavior of the metric values as the iterations progress.

  const unsigned int numberOfPixels = fixedImageRegion.GetNumberOfPixels();
 
  const unsigned int numberOfSamples =
                        static_cast< unsigned int >( numberOfPixels * SAMPLE_FRACTION );
 
  metric->SetNumberOfSpatialSamples( numberOfSamples );
 
  //optimizer->SetLearningRate( 15.0 ); //"All the sampled point mapped to outside of the moving image"
  //optimizer->SetLearningRate( 1.0 );
  optimizer->SetLearningRate( LEARNING_RATE );
  optimizer->SetNumberOfIterations( MAX_ITERATIONS );

#if MUTUAL_INFO == 1
  optimizer->MaximizeOn(); // We want to maximize mutual information (the default of the optimizer is to minimize)
#else
  optimizer->MinimizeOn();
#endif
  
  // Note that large values of the learning rate will make the optimizer
  // unstable. Small values, on the other hand, may result in the optimizer
  // needing too many iterations in order to walk to the extrema of the cost
  // function. The easy way of fine tuning this parameter is to start with
  // small values, probably in the range of $\{5.0,10.0\}$. Once the other
  // registration parameters have been tuned for producing convergence, you
  // may want to revisit the learning rate and start increasing its value until
  // you observe that the optimization becomes unstable.  The ideal value for
  // this parameter is the one that results in a minimum number of iterations
  // while still keeping a stable path on the parametric space of the
  // optimization. Keep in mind that this parameter is a multiplicative factor
  // applied on the gradient of the Metric. Therefore, its effect on the
  // optimizer step length is proportional to the Metric values themselves.
  // Metrics with large values will require you to use smaller values for the
  // learning rate in order to maintain a similar optimizer behavior.
 
  try
    {
      start_timer = std::clock();
	std::cout << "registration starting..." << std::endl;
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
 
  std::cout << "Final Parameters: " << finalParameters << std::endl;
 
  unsigned int numberOfIterations = optimizer->GetCurrentIteration();
 
  double bestValue = optimizer->GetValue();
 
  // Print out results
  std::cout << std::endl;
  std::cout << "Result = " << std::endl;
  std::cout << " Iterations    = " << numberOfIterations << std::endl;
  std::cout << " Metric value  = " << bestValue          << std::endl;
  std::cout << " Numb. Samples = " << numberOfSamples    << std::endl;
 
  typedef itk::ResampleImageFilter<
                            ImageType,
                            ImageType >    ResampleFilterType;
 
  TransformType::Pointer finalTransform = TransformType::New();
 
  finalTransform->SetParameters( finalParameters );
  finalTransform->SetFixedParameters( transform->GetFixedParameters() );
 
  ResampleFilterType::Pointer resample = ResampleFilterType::New();
 
  resample->SetTransform( finalTransform );
  resample->SetInput( movingDicomReader->GetOutput());
 
  resample->SetSize(    fixedDicomReader->GetOutput()->GetLargestPossibleRegion().GetSize() );
  resample->SetOutputOrigin(  fixedDicomReader->GetOutput()->GetOrigin() );
  resample->SetOutputSpacing( fixedDicomReader->GetOutput()->GetSpacing() );
  resample->SetOutputDirection( fixedDicomReader->GetOutput()->GetDirection() );
  resample->SetDefaultPixelValue( DEFAULT_PIXEL_VALUE );
  
  //////////////Begin Dicom Writer
  typedef itk::ImageFileWriter< ImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();

   writer->SetFileName( "output.dcm" );

   writer->SetInput( resample->GetOutput() );

   writer->Update();

  //////////////End Dicom Writer
  
  /*Writer for 2d
  typedef itk::ImageFileWriter< ImageType >  WriterType;
  WriterType::Pointer      writer =  WriterType::New();
  writer->SetFileName("output.dcm");
  writer->SetInput( resample->GetOutput()   );
  writer->Update();
  */
 
  return EXIT_SUCCESS;
}