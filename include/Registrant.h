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
  };
}
#endif
