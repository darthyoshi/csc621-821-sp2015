#ifndef QUANTIFIER_H
#define QUANTIFIER_H

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
#include <QVTKWidget.h>
#include <QFileDialog>
#include <string>

#include "itkImage.h"
#include "itkImageSeriesReader.h"
#include "itkImageToVTKImageFilter.h"
#include "itkLabelToRGBImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkLabelStatisticsImageFilter.h"
#include "itkScalarConnectedComponentImageFilter.h"

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkResliceImageViewer.h>
#include <vtkImageSlabReslice.h>
#include <vtkCommand.h>
#include <vtkResliceCursorActor.h>
#include <vtkResliceCursorThickLineRepresentation.h>
#include <vtkImageMarchingCubes.h>
#include <vtkResliceCursorWidget.h>
#include <vtkResliceCursorLineRepresentation.h>
#include <vtkImagePlaneWidget.h>
#include <vtkImageData.h>
#include <vtkImageActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkCellPicker.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkProperty.h>
#include <vtkResliceCursorPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkCamera.h>

#include "easylogging++.h"

#include "Common.h"
#include "Stage.h"

namespace vis {

  class Quantifier : public Stage {

    Q_OBJECT

    protected:
      typedef itk::ImageSeriesReader<BaseImage> Reader;

      // used to create the type of images
      typedef itk::RGBPixel<unsigned char> RGBPixelType;
      typedef itk::Image<unsigned int, 3> LabelImage;

      // used to create each connected component
      typedef itk::ScalarConnectedComponentImageFilter <BaseImage, LabelImage> BlobDetector;

      // used to create data for each connected component
      typedef itk::RelabelComponentImageFilter <LabelImage, LabelImage> RelabelFilter;

      // used to create the colored filters to show each connected component
      typedef itk::LabelToRGBImageFilter<LabelImage, itk::Image<itk::RGBPixel<unsigned char>, 3>> RGBFilter;

      // used to connect ITK pipeline to VTK
      typedef itk::ImageToVTKImageFilter<itk::Image<RGBPixelType, 3>> Converter;

      // used to create image statistics
      typedef itk::LabelStatisticsImageFilter< BaseImage, LabelImage > LabelStatistics;

    public:
      Quantifier();

      QWidget* GetToolbox();
      QWidget* GetContent();

    public slots:
      void SetImage(BaseImage::Pointer);
      void ToggleMode();
      void UpdateSlice();
      void UpdateThreshold();
      void UpdateSize();
      void ResetCamera();

    protected:
      void BuildToolbox();
      void BuildContent();

      LabelStatistics::Pointer Quantifier::GetStatistics();

    private:
      vtkRenderer* m_renderer;
      vtkRenderWindowInteractor* m_interactor;
      vtkImageViewer2* m_imageView;
      Converter::Pointer m_converter;
      LabelStatistics::Pointer m_statistics;

      QWidget* m_toolBox;
      QVTKWidget* m_view;

      QLabel* m_modeLabel;
      QLabel* m_sliceLabel;
      QLabel* m_sliceValueLabel;
      QLabel* m_thresholdLabel;
      QLabel* m_sizeLabel;

      QSlider* m_sliceSlider;
      QSlider* m_thresholdSlider;
      QSlider* m_sizeSlider;

      bool m_3Dmode;

      //filters
      BlobDetector::Pointer m_connector;
      RelabelFilter::Pointer m_relabel;
      RGBFilter::Pointer m_rgbFilter;

      const std::string m_labels[2] = {"MIP","Cine-View"};
  };
}

#endif
