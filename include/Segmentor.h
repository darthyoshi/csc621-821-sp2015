#ifndef SEGMENTOR_H
#define SEGMENTOR_H

#include <QWidget>
#include <QObject>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QSlider>
#include <QGroupBox>
#include <QPushButton>
#include <QAbstractState>
#include <QFontMetrics>
#include <QSlider>
#include <QVTKWidget.h>
#include <QFileDialog>

#include <string>
#include <limits>

#include "itkImage.h"
#include "itkImageToVTKImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h"

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkResliceImageViewer.h>
#include <vtkImageSlabReslice.h>
#include <vtkImageViewer2.h>
#include <vtkCommand.h>
#include <vtkResliceCursorActor.h>
#include <vtkResliceCursorThickLineRepresentation.h>
#include <vtkImageMarchingCubes.h>
#include <vtkResliceCursorWidget.h>
#include <vtkResliceCursorLineRepresentation.h>
#include <vtkImagePlaneWidget.h>
#include <vtkImageData.h>
#include <vtkPolyDataMapper.h>
#include <vtkCellPicker.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkProperty.h>
#include <vtkResliceCursorPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>

#include "easylogging++.h"

#include "Common.h"
#include "Stage.h"

namespace vis {

  class Segmentor : public Stage {

    Q_OBJECT

    protected:
      typedef itk::SubtractImageFilter<
        BaseImage, BaseImage, BaseImage
      > SubtractFilter;
      typedef itk::BinaryThresholdImageFilter<
        BaseImage, BaseImage
      > ThresholdFilter;
      typedef itk::BinaryBallStructuringElement<
        BasePixel, 3
      > StructuringElement;
      typedef itk::BinaryErodeImageFilter<
        BaseImage, BaseImage, StructuringElement 
      >  ErodeFilter;
      typedef itk::BinaryDilateImageFilter<
        BaseImage, BaseImage, StructuringElement 
      >  DilateFilter;
      typedef itk::ImageToVTKImageFilter<BaseImage> Converter;

    private:
      SubtractFilter::Pointer m_subtract;
      Converter::Pointer m_converter;
      ThresholdFilter::Pointer m_threshold;
      ErodeFilter::Pointer  m_erode;
      DilateFilter::Pointer m_dilate;
      StructuringElement m_structuringElement;

      vtkRenderer* m_renderer;
      vtkRenderWindow* m_renderWindow;
      vtkImagePlaneWidget* m_plane;
      vtkRenderWindowInteractor* m_interactor;

      QWidget* m_toolBox;
      QVTKWidget* m_view;

      QLabel* m_minLabel;
      QLabel* m_maxLabel;
      QLabel* m_openingLabel;
      QSlider* m_minSlider;
      QSlider* m_maxSlider;
      QSlider* m_openingSlider;

      int m_openingRadius;

      bool m_resetView;

      void BuildToolbox();
      void BuildContent();
      void UpdateView();

    public:
      Segmentor();
      void Segment();

      QWidget* GetContent();
      QWidget* GetToolbox();

    signals:
      void SegmentationComplete(BaseImage::Pointer);

    public slots:
	    void SetFixedImage(BaseImage::Pointer);
	    void SetMovingImage(BaseImage::Pointer);
  };

}

#endif
