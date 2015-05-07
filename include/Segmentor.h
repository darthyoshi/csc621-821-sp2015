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
#include <QVTKWidget.h>
#include <QFileDialog>
#include <string>

#include "itkImage.h"
#include "itkImageToVTKImageFilter.h"
#include "itkSubtractImageFilther.h"

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
    	typedef itk::SubtractImageFilter<ImageType, ImageType, ImageType> SubtractFilter;

    private:
      SubtractFilter* m_SubtractFilter;
      vtkRenderer* m_renderer;
      vtkRenderWindow* m_renderWindow;
      vtkRenderWindowInteractor* m_interactor;
      vtkImagePlaneWidget* m_planes[3];
      vtkImageMarchingCubes* m_surface;
      vtkPolyDataMapper* m_mapper;
      Converter::Pointer m_converter;

      QWidget* m_toolBox;
      QVTKWidget* m_view;

      QLabel* m_UIDLabel;
      QLabel* m_slicesLabel;

      void BuildToolbox();
      void BuiltContent();

    public:
      Segmentor();

      QWidget* GetContent();
      QWidget* GetToolbox();

    public slots:
	    void SetFixedImage (BaseImage::Pointer);
	    void SetMovingImage (BaseImage::Pointer);

  };

}

#endif
