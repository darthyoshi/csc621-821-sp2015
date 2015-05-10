#ifndef LOADER_H
#define LOADER_H

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

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkResliceImageViewer.h>
#include <vtkImageSlabReslice.h>
#include <vtkCommand.h>
#include <vtkResliceCursorActor.h>
#include <vtkResliceCursorThickLineRepresentation.h>
#include <vtkImageMarchingCubes.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkResliceCursorWidget.h>
#include <vtkResliceCursorLineRepresentation.h>
#include <vtkImagePlaneWidget.h>
#include <vtkImageData.h>
#include <vtkPolyDataMapper.h>
#include <vtkCellPicker.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkProperty.h>
#include <vtkVolumeProperty.h>
#include <vtkResliceCursorPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>

#include "easylogging++.h"

#include "Common.h"
#include "Stage.h"

namespace vis {

  class Loader : public Stage {

    Q_OBJECT

    protected:
      typedef itk::ImageSeriesReader<BaseImage> Reader;
      typedef itk::ImageToVTKImageFilter<BaseImage> Converter;

    public:
      Loader();

      QWidget* GetToolbox();
      QWidget* GetContent();

    signals:
      void SourceChanged(BaseImage::Pointer);

    public slots:
      void LoadDICOMSource();

    protected:
      void BuildToolbox();
      void BuildContent();
      void UpdateView();
      //void UpdateWidgets();

    private:
      Reader::Pointer m_reader;
      vtkRenderer* m_renderer;
      vtkRenderWindow* m_renderWindow;
      vtkRenderWindowInteractor* m_interactor;
      vtkImagePlaneWidget* m_planes[3];
      vtkSmartVolumeMapper* m_mapper;
      vtkVolumeProperty* m_property;
      vtkVolume* m_volume;
      Converter::Pointer m_converter;

      QWidget* m_toolBox;
      QVTKWidget* m_view;

      QLabel* m_UIDLabel;
      QLabel* m_slicesLabel;
  };
}

#endif
