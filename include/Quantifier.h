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
#include <vtkCommand.h>
#include <vtkImageData.h>
#include <vtkImageActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkCellPicker.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkProperty.h>
#include <vtkResliceCursorPolyDataAlgorithm.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkSmartPointer.h>
#include <vtkCamera.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkVolumeProperty.h>

#include "easylogging++.h"

#include "Common.h"
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
      typedef itk::LabelToRGBImageFilter<LabelImage, ColorImage> ColorizeFilter;
      typedef itk::ImageToVTKImageFilter<ColorImage> Converter;
      typedef itk::LabelStatisticsImageFilter<
        BaseImage, LabelImage
      > LabelStatistics;

    public:
      Quantifier();

      QWidget* GetToolbox();
      QWidget* GetContent();

    public slots:
      void SetImage(BaseImage::Pointer);

    protected:
      void BuildToolbox();
      void BuildContent();
      void Quantify();
      void UpdateView();

    private:
      //ITK/VTK base
      vtkRenderer* m_renderer;
      vtkRenderWindowInteractor* m_interactor;
      Converter::Pointer m_converter;

      //UI
      QWidget* m_toolBox;
      QVTKWidget* m_view;

      //blob detection
      BlobDetector::Pointer m_connector;
      RelabelFilter::Pointer m_relabel;
      ColorizeFilter::Pointer m_colorize;
      LabelStatistics::Pointer m_statistics;

      // Volume rendering.
      vtkSmartVolumeMapper* m_mapper;
      vtkVolumeProperty* m_property;
      vtkVolume* m_volume;
  };
}

#endif
