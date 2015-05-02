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

#include "itkMultiResolutionImageRegistrationMethod.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkRegularStepGradientDescentOptimizer.h"
#include "itkVersorRigid3DTransform.h"
#include "itkCenteredTransformInitializer.h"
#include "itkVersorRigid3DTransformOptimizer.h"
#include "itkImage.h"
#include "itkImageFileWriter.h"
#include "itkImageSeriesReader.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkCheckerBoardImageFilter.h"
#include "itkCommand.h"

#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkImagePlaneWidget.h>

#include "itkImageSeriesReader.h"
#include "itkImageToVTKImageFilter.h"

#include "Stage.h"
#include "RegistrationIterationCommand.h"
#include "RegistrationInterfaceCommand.h"
#include "Common.h"

namespace vis {

  class Registrant : public Stage {

    Q_OBJECT

    protected:
      typedef itk::ImageSeriesReader<BaseImage> Reader;
      typedef itk::ImageToVTKImageFilter<BaseImage> Converter;
      typedef itk::Image<BasePixel, 3> InternalImage;
      typedef itk::VersorRigid3DTransform<double> Transform;
      typedef itk::VersorRigid3DTransformOptimizer Optimizer;
      typedef itk::LinearInterpolateImageFunction<
        InternalImage, double
      > Interpolator;
      typedef itk::MattesMutualInformationImageToImageMetric<
        InternalImage, InternalImage
      > Metric;
      typedef itk::MultiResolutionImageRegistrationMethod<
        InternalImage, InternalImage 
      > Registration;
      typedef itk::MultiResolutionPyramidImageFilter<
        InternalImage, InternalImage
      > ImagePyramid;
      typedef itk::CastImageFilter<BaseImage, InternalImage> CastFilter;
      typedef itk::ResampleImageFilter<BaseImage, BaseImage> ResampleFilter;
      typedef itk::CheckerBoardImageFilter<BaseImage> CheckerBoardFilter;

    public:
      Registrant();

      QWidget* GetToolbox();
      QWidget* GetContent();

    signals:
      void RegistrationComplete(BaseImage::Pointer);

    public slots:
      void SetFixedSource(BaseImage::Pointer);
      void LoadMovingImage();
      void Register();

    protected:
      void BuildToolbox();
      void BuildContent();
      void UpdateView();

    private:
      int m_histogramBins = 128;
      int m_histogramSamples = 50000;
      int m_iterations = 200;
      double m_relaxationFactor = 0.9;
      int m_levels = 3;
      BasePixel m_greyLevel = 0;

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

      BaseImage::Pointer m_fixedImage;
      Transform::Pointer m_transform;
      Optimizer::Pointer m_optimizer;
      Interpolator::Pointer m_interpolator;
      Registration::Pointer m_registration;
      Metric::Pointer m_metric;
      ImagePyramid::Pointer m_fixedImagePyramid;
      ImagePyramid::Pointer m_movingImagePyramid;
      CastFilter::Pointer m_fixedCaster;
      CastFilter::Pointer m_movingCaster;
      Transform::Pointer m_finalTransform;
      ResampleFilter::Pointer m_resample;
      CheckerBoardFilter::Pointer m_checkerBoard;

  };
}
#endif
