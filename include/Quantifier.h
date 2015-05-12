#ifndef QUANTIFIER_H
#define QUANTIFIER_H

#include <QWidget>
#include <QObject>
#include <QGridLayout>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QColor>
#include <QSlider> 
#include <QGroupBox>
#include <QPushButton>
#include <QAbstractState>
#include <QFontMetrics>
#include <QVTKWidget.h>
#include <QFileDialog>
#include <string>

#include "itkImage.h"
#include "itkImageSource.h"
#include "itkImageRegion.h"
#include "itkImageSeriesReader.h"
#include "itkDefaultConvertPixelTraits.h"
#include "itkConvertPixelBuffer.h"
#include "itkImageToVTKImageFilter.h"
#include "itkLabelToRGBImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkLabelStatisticsImageFilter.h"
#include "itkMersenneTwisterRandomVariateGenerator.h"
#include "itkLabelStatisticsImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkScalarConnectedComponentImageFilter.h"

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>
#include <vtkBoxWidget2.h>
#include <vtkImageData.h>
#include <vtkImageIterator.h>
#include <vtkImageActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkCellPicker.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkProperty.h>
#include <vtkResliceCursorPolyDataAlgorithm.h>
#include <vtkImagePlaneWidget.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkSmartPointer.h>
#include <vtkCamera.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkVolumeProperty.h>

#include "easylogging++.h"

#include "Common.h"
#include "BoxWidgetCallback.h"
#include "Stage.h"

namespace vis {

  class Quantifier : public Stage {

    Q_OBJECT

    protected:
      typedef itk::ScalarConnectedComponentImageFilter<
        BaseImage, LabelImage
      > BlobDetector;
      typedef itk::RelabelComponentImageFilter<
        LabelImage, LabelImage
      > RelabelFilter;
      typedef itk::ImageToVTKImageFilter<LabelImage> Converter;
      typedef itk::LabelStatisticsImageFilter<
        BaseImage, LabelImage
      > LabelStatistics;
      typedef itk::Statistics::MersenneTwisterRandomVariateGenerator 
        VariateGenerator;
      typedef typename LabelStatistics::ValidLabelValuesContainerType LabelSet;

    public:
      Quantifier();

      QWidget* GetToolbox();
      QWidget* GetContent();

    public slots:
      void SetImage(BaseImage::Pointer);
      void SelectComponent(int index);

    protected:
      void BuildToolbox();
      void BuildContent();
      void Quantify();
      void UpdateInterface();
      void UpdateView();

    private:
      //ITK/VTK base
      vtkRenderer* m_renderer;
      vtkRenderWindowInteractor* m_interactor;
      Converter::Pointer m_converter;
      VariateGenerator::Pointer m_rng;

      //UI
      QWidget* m_toolBox;
      QVTKWidget* m_view;
      QComboBox* m_combo;

      QLabel* m_idLabel;
      QLabel* m_meanLabel;
      QLabel* m_sigmaLabel;
      QLabel* m_varianceLabel;
      QLabel* m_sumLabel;
      QLabel* m_sizeLabel;

      LabelPixel m_lastSelected = 0;

      //blob detection
      BlobDetector::Pointer m_connector;
      RelabelFilter::Pointer m_relabel;
      LabelStatistics::Pointer m_statistics;

      // Volume rendering.
      vtkSmartVolumeMapper* m_mapper;
      vtkVolumeProperty* m_property;
      vtkColorTransferFunction* m_color;
      vtkPiecewiseFunction* m_opacity;
      vtkVolume* m_volume;
      vtkBoxWidget2* m_box;
  };
}

#endif
