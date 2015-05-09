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

#include "easylogging++.h"

#include "Common.h"
#include "Stage.h"

namespace vis {

  class Quantifier : public Stage {

    Q_OBJECT

    protected:
      typedef itk::ImageSeriesReader<BaseImage> Reader;
      typedef itk::ImageToVTKImageFilter<BaseImage> Converter;

    public:
      Quantifier();

      QWidget* GetToolbox();
      QWidget* GetContent();

//    signals:
//      void SourceChanged(BaseImage::Pointer);

    public slots:
      void UpdateImage(BaseImage::Pointer);
      void ToggleMode();

    protected:
      void BuildToolbox();
      void BuildContent();
      void UpdateView();
      //void UpdateWidgets();

    private:
      vtkRenderer* m_renderer;
      vtkRenderWindowInteractor* m_interactor;
      Converter::Pointer m_converter;
      vtkSmartPointer<vtkImageViewer2> m_viewer;

      QWidget* m_toolBox;
      QVTKWidget* m_view;

      QLabel* m_modeLabel;
      QLabel* m_slicesLabel;

      int m_currentSlice;
      bool m_3Dmode;

      const std::string m_labels[2] = {"Maximum Intensity Projection","Cine-View"};
  };
}

#endif
