#ifndef REGISTRANT_H
#define REGISTRANT_H

#include "easylogging++.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QDir>
#include <QFileDialog>
#include <QFileInfoList>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QVTKWidget.h>

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

#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkImagePlaneWidget.h>

#include "itkImageSeriesReader.h"
#include "itkImageToVTKImageFilter.h"

#include "Stage.h"
#include "Common.h"

namespace vis {

  class Registrant : public Stage {

    Q_OBJECT

    protected:
      typedef itk::ImageSeriesReader<BaseImage> Reader;
      typedef itk::ImageToVTKImageFilter<BaseImage> Converter;

    public:
      Registrant();

      QWidget* GetToolbox();
      QWidget* GetContent();

    signals:
      void RegistrationComplete();

    public slots:
      void SetFixedSource(BaseImage::Pointer);
      void LoadMovingImage();
      void Update();

    protected:
      void BuildToolbox();
      void BuildContent();
      void UpdateView();

    private:
      Reader::Pointer m_reader;
      vtkRenderer* m_renderer;
      vtkRenderWindow* m_renderWindow;
      vtkRenderWindowInteractor* m_interactor;
      Converter::Pointer m_converter;

      QWidget* m_toolBox;
      QVTKWidget* m_view;

      // Interface Elements
      QLabel* m_UIDLabel;
      QLabel* m_slicesLabel;

      // need to know the reader and writer image Types to cast them to the internal pixel type
      // static const   unsigned int    Dimension = 3;
      // typedef  unsigned short  PixelType;
      // typedef itk::Image< PixelType, Dimension >  FixedImageType;
      // typedef itk::Image< PixelType, Dimension >  MovingImageType;
      typedef BaseImage FixedImageType;
      typedef BaseImage MovingImageType;
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
      typedef itk::CastImageFilter< FixedImageType, InternalImageType > FixedCastFilterType;
      typedef itk::CastImageFilter< MovingImageType, InternalImageType > MovingCastFilterType;
      typedef itk::ResampleImageFilter< MovingImageType, FixedImageType > ResampleFilterType;
      typedef  unsigned short  OutputPixelType;
      typedef itk::Image< OutputPixelType, Dimension > OutputImageType;
      typedef itk::CastImageFilter< FixedImageType, OutputImageType > CastFilterType;
      typedef itk::CheckerBoardImageFilter< FixedImageType > CheckerBoardFilterType;
  
      FixedImageType::Pointer           fixedImage;
      TransformType::Pointer            transform;
      OptimizerType::Pointer            optimizer;
      InterpolatorType::Pointer         interpolator;
      RegistrationType::Pointer         registration;
      MetricType::Pointer               metric;
      FixedImagePyramidType::Pointer    fixedImagePyramid;
      MovingImagePyramidType::Pointer   movingImagePyramid;
      FixedCastFilterType::Pointer      fixedCaster;
      MovingCastFilterType::Pointer     movingCaster;
      TransformType::Pointer            finalTransform;
      ResampleFilterType::Pointer       resample;
      CastFilterType::Pointer           caster;
      CheckerBoardFilterType::Pointer   checker;
      OutputImageType::Pointer          finalImage;
      OutputImageType::Pointer          beforeRegisterCheckerImage;
      OutputImageType::Pointer          afterRegisterCheckerImage;

  };
}
#endif
