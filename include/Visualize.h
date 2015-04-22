#ifndef VISUALIZE_H
#define VISUALIZE_H

#include <vector>
#include <type_traits>

#include <QObject>
#include <QMainWindow>
#include <QToolBox>
#include <QGridLayout>

#include <QtCore/qstatemachine.h>
#include <QtWidgets/qtoolbox.h>
#include <vtkResliceImageViewer.h>
#include <vtkSmartPointer.h>
#include <vtkImagePlaneWidget.h>
#include <vtkResliceCursorWidget.h>
#include <vtkResliceCursorActor.h>
#include <vtkResliceCursorPolyDataAlgorithm.h>
#include <vtkCellPicker.h>
#include <vtkResliceCursorLineRepresentation.h>
#include <vtkPointHandleRepresentation2D.h>
#include <vtkResliceCursor.h>
#include <vtkPlane.h>
#include <vtkDistanceRepresentation2D.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkResliceImageViewerMeasurements.h>
#include <vtkPlaneSource.h>
#include <vtkScalarsToColors.h>
#include <vtkImageMapToColors.h>
#include <vtkDistanceWidget.h>
#include <vtkProperty.h>
#include <vtkPointPlacer.h>
#include <vtkRenderer.h>
#include <vtkCommand.h>

#include "itkImage.h"
#include "itkSmartPointer.h"
#include "itkRGBPixel.h"

#include "VisualizeImageSource.h"
#include "Loader.h"
#include "Registrant.h"
#include "Segmentor.h"
#include "Common.h"

#include "ui_Window.h"

namespace Ui { class MainWindow; }

namespace vis {

  class Visualize : public QMainWindow {

    Q_OBJECT

    public:
      enum RenderView {
        CORONAL_VIEW,
        AXIAL_VIEW,
        SAGITTAL_VIEW,
      };

    public: 
      Visualize(QWidget* parent = 0);
      ~Visualize();

      VisualizeImageSource* GetImageSource();

    public slots:
      virtual void Render();
      virtual void AddDistanceToView(int);
      virtual void ResetViews();
      virtual void Update();

    protected:
      Ui::MainWindow* m_window;
      QToolBox* m_toolbox;

      VisualizeImageSource::Pointer m_imageData;
      vtkSmartPointer<vtkResliceImageViewer> m_viewers[4];
      vtkSmartPointer<vtkDistanceWidget> m_distanceWidgets[4];
      vtkSmartPointer<vtkImagePlaneWidget> m_planeWidgets[4];
      vtkSmartPointer<vtkResliceImageViewerMeasurements> m_measurements[4];
      itk::ImageToVTKImageFilter<VisualizeImageSource::OutputImageType>::Pointer m_converter;

      Loader* m_loader;
      Registrant* m_registrant;
      Segmentor* m_segmentor;
  };
}

#endif
