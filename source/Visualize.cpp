#include "Visualize.h"

namespace vis {

  class vtkResliceCursorCallback : public vtkCommand {
    public:
      static vtkResliceCursorCallback* New() {
        return new vtkResliceCursorCallback;
      }

      void Execute(vtkObject* caller, unsigned long ev, void* data) {
        if (ev == vtkResliceCursorWidget::WindowLevelEvent ||
            ev == vtkCommand::WindowLevelEvent ||
            ev == vtkResliceCursorWidget::ResliceThicknessChangedEvent)
        {
          for (int i = 0; i < 3; i++) {
            this->cursors[i]->Render();
          }
          this->planes[0]->GetInteractor()->GetRenderWindow()->Render();
          return;
        }

        vtkImagePlaneWidget* ipw = dynamic_cast<vtkImagePlaneWidget*>(caller);
        if (ipw) {
          double* wl = static_cast<double*>(data);
          if (ipw == this->planes[0]) {
            this->planes[1]->SetWindowLevel(wl[0], wl[1], 1);
            this->planes[2]->SetWindowLevel(wl[0], wl[1], 1);
          } else if (ipw == this->planes[1]) {
            this->planes[0]->SetWindowLevel(wl[0], wl[1], 1);
            this->planes[2]->SetWindowLevel(wl[0], wl[1], 1);
          } else if (ipw == this->planes[2]) {
            this->planes[0]->SetWindowLevel(wl[0], wl[1], 1);
            this->planes[1]->SetWindowLevel(wl[0], wl[1], 1);
          }
        }

        vtkResliceCursorWidget* rcw =
          dynamic_cast<vtkResliceCursorWidget*>(caller);
        if (rcw) {
          vtkResliceCursorLineRepresentation* rep =
            dynamic_cast<vtkResliceCursorLineRepresentation*>(rcw->GetRepresentation());
          rep->GetResliceCursorActor()->GetCursorAlgorithm()->GetResliceCursor();
          for (int i = 0; i < 3; i++) {
            auto alg = this->planes[i]->GetPolyDataAlgorithm();
            vtkPlaneSource* ps = static_cast<vtkPlaneSource*>(alg);
            ps->SetOrigin(this->cursors[i]->GetResliceCursorRepresentation()->
              GetPlaneSource()->GetOrigin());
            ps->SetPoint1(this->cursors[i]->GetResliceCursorRepresentation()->
              GetPlaneSource()->GetPoint1());
            ps->SetPoint2(this->cursors[i]->GetResliceCursorRepresentation()->
              GetPlaneSource()->GetPoint2());
            this->planes[i]->UpdatePlacement();
          }
        }

        for (int i = 0; i < 3; i++) {
          this->cursors[i]->Render();
        }
        this->planes[0]->GetInteractor()->GetRenderWindow()->Render();
      }

    public:
      vtkResliceCursorCallback() {}
      vtkImagePlaneWidget* planes[3];
      vtkResliceCursorWidget* cursors[3];
  };
}

using namespace vis;

Visualize::Visualize(QWidget* parent = 0) : QMainWindow(parent),
  m_window(new Ui::MainWindow()) 
{
  m_window->setupUi(this);

  QToolBox* toolbox = new QToolBox();
  m_window->sideBar->layout()->addWidget(toolbox);

  // Master state machine for the application.
  m_loader = new Loader();
  int loadIndex = toolbox->addItem(m_loader, tr("Load"));

  m_registrant = new Registrant();
  int registerIndex = toolbox->addItem(m_registrant, tr("Register"));

  m_segmentor = new Segmentor();
  int segmentIndex = toolbox->addItem(m_segmentor, tr("Segment"));

  // Initialize image data.
  m_imageData = VisualizeImageSource::New();

  typedef VisualizeImageSource::OutputImageType OutType;
  m_converter = itk::ImageToVTKImageFilter<OutType>::New();

  for (int i = 0; i < 3; i++) {
    m_viewers[i] = vtkSmartPointer<vtkResliceImageViewer>::New();
  }

  /**
   * Sagittal View Setup
   */
  auto sagittal = m_viewers[SAGITTAL_VIEW];
  this->m_window->sagittalView->SetRenderWindow(sagittal->GetRenderWindow());

  auto sagInter = this->m_window->sagittalView->GetRenderWindow()->GetInteractor();
  sagittal->SetupInteractor(sagInter);

  /**
   * Coronal View Setup
   */
  auto coronal = m_viewers[CORONAL_VIEW];
  this->m_window->coronalView->SetRenderWindow(coronal->GetRenderWindow());

  auto corInter = this->m_window->coronalView->GetRenderWindow()->GetInteractor();
  coronal->SetupInteractor(corInter);

  /**
   * Axial View Setup
   */
  auto axial = m_viewers[AXIAL_VIEW];
  this->m_window->axialView->SetRenderWindow(axial->GetRenderWindow());

  auto axialInter = this->m_window->axialView->GetRenderWindow()->GetInteractor();
  axial->SetupInteractor(axialInter);

  m_converter->SetInput(m_imageData->GetOutput());
  m_converter->Update();

  for (int i = 0; i < 3; i++) {
    vtkResliceCursorLineRepresentation* rep = 
      vtkResliceCursorLineRepresentation::SafeDownCast(
        m_viewers[i]->GetResliceCursorWidget()->GetRepresentation());
    m_viewers[i]->SetResliceCursor(m_viewers[0]->GetResliceCursor());

    rep->GetResliceCursorActor()->GetCursorAlgorithm()->SetReslicePlaneNormal(i);

    m_viewers[i]->SetInputData(m_converter->GetOutput());
    m_viewers[i]->SetSliceOrientation(i);
    m_viewers[i]->SetResliceModeToAxisAligned();
  }

  vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
  picker->SetTolerance(0.005);

  vtkSmartPointer<vtkProperty> planeProp = vtkSmartPointer<vtkProperty>::New();
  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();

  this->m_window->perspectiveView->GetRenderWindow()->AddRenderer(renderer);
  vtkRenderWindowInteractor* oblInter = 
    this->m_window->perspectiveView->GetInteractor();

  for (int i = 0; i < 3; i++) {
    m_planeWidgets[i] = vtkSmartPointer<vtkImagePlaneWidget>::New();
    m_planeWidgets[i]->SetInteractor(oblInter);
    m_planeWidgets[i]->SetPicker(picker);
    m_planeWidgets[i]->RestrictPlaneToVolumeOn();
    
    double color[3] = {0, 0, 0};
    color[i] = 1;
    m_planeWidgets[i]->GetPlaneProperty()->SetColor(color);

    color[0] /= 4.0;
    color[1] /= 4.0;
    color[2] /= 4.0;

    m_viewers[i]->GetRenderer()->SetBackground(color);

    m_planeWidgets[i]->SetTexturePlaneProperty(planeProp);
    m_planeWidgets[i]->TextureInterpolateOff();
    m_planeWidgets[i]->SetResliceInterpolateToLinear();
    m_planeWidgets[i]->SetInputData(m_converter->GetOutput());
    m_planeWidgets[i]->SetPlaneOrientation(i);
    m_planeWidgets[i]->SetSliceIndex(0);
    m_planeWidgets[i]->DisplayTextOn();
    m_planeWidgets[i]->SetDefaultRenderer(renderer);
    m_planeWidgets[i]->SetWindowLevel(1358, -27);
    m_planeWidgets[i]->On();
    m_planeWidgets[i]->InteractionOn();
  }

  vtkSmartPointer<vtkResliceCursorCallback> cb =
    vtkSmartPointer<vtkResliceCursorCallback>::New();

  for (int i = 0; i < 3; i++) {
    cb->planes[i] = m_planeWidgets[i];
    cb->cursors[i] = m_viewers[i]->GetResliceCursorWidget();

    m_viewers[i]->GetResliceCursorWidget()->AddObserver(
      vtkResliceCursorWidget::ResliceAxesChangedEvent, cb);
    m_viewers[i]->GetResliceCursorWidget()->AddObserver(
      vtkResliceCursorWidget::WindowLevelEvent, cb);
    m_viewers[i]->GetResliceCursorWidget()->AddObserver(
      vtkResliceCursorWidget::ResliceThicknessChangedEvent, cb);
    m_viewers[i]->GetResliceCursorWidget()->AddObserver(
      vtkResliceCursorWidget::ResetCursorEvent, cb);
    m_viewers[i]->GetResliceCursorWidget()->AddObserver(
      vtkCommand::WindowLevelEvent, cb);

    m_viewers[i]->SetLookupTable(m_viewers[0]->GetLookupTable());

    m_planeWidgets[i]->GetColorMap()->SetLookupTable(
      m_viewers[0]->GetLookupTable());
    m_planeWidgets[i]->SetColorMap(
      m_viewers[i]->GetResliceCursorWidget()->GetResliceCursorRepresentation()->GetColorMap());
  }
  m_viewers[0]->SetAlphaRange(0.0, 1.0);
  m_viewers[0]->Build();

  m_viewers[0]->GetLookupTable()->PrintSelf(std::cout, vtkIndent(1));

  this->m_window->sagittalView->show();
  this->m_window->coronalView->show();
  this->m_window->axialView->show();

  typedef Loader::Reader Reader;
  connect(m_loader, &Loader::SourceChanged, [=](BaseImage::Pointer p) {
    m_converter->SetInput(p);
    Update();
  });
  connect(m_window->actionExit, SIGNAL(triggered()), this, SLOT(close()));
}

Visualize::~Visualize() {
  delete m_window; 
}

VisualizeImageSource* Visualize::GetImageSource() {
  return m_imageData;
}

void Visualize::Render() {
  for (int i = 0; i < 3; i++) {
    m_viewers[i]->Render();
  }
  this->m_window->perspectiveView->GetRenderWindow()->Render();
}

void Visualize::AddDistanceToView(int i) {
  if(m_distanceWidgets[i]) {
    m_distanceWidgets[i]->SetEnabled(0);
    m_distanceWidgets[i] = NULL;
  }

  m_distanceWidgets[i] = vtkSmartPointer<vtkDistanceWidget>::New();
  m_distanceWidgets[i]->SetInteractor(
    m_viewers[i]->GetResliceCursorWidget()->GetInteractor());
  m_distanceWidgets[i]->SetPriority(
    m_viewers[i]->GetResliceCursorWidget()->GetPriority() + 0.01);

  vtkSmartPointer<vtkPointHandleRepresentation2D> handle =
    vtkSmartPointer<vtkPointHandleRepresentation2D>::New();
  vtkSmartPointer<vtkDistanceRepresentation2D> distance =
    vtkSmartPointer<vtkDistanceRepresentation2D>::New();

  distance->SetHandleRepresentation(handle);
  m_distanceWidgets[i]->SetRepresentation(distance);

  distance->InstantiateHandleRepresentation();
  distance->GetPoint1Representation()->SetPointPlacer(
    (vtkPointPlacer*)m_viewers[i]->GetPointPlacer());
  distance->GetPoint2Representation()->SetPointPlacer(
    (vtkPointPlacer*)m_viewers[i]->GetPointPlacer());

  m_viewers[i]->GetMeasurements()->AddItem(m_distanceWidgets[i]);

  m_distanceWidgets[i]->CreateDefaultRepresentation();
  m_distanceWidgets[i]->EnabledOn();
}

void Visualize::Update() {
  m_converter->Update();
  for (int i = 0; i < 3; i++) {
    m_viewers[i]->SetInputData(m_converter->GetOutput());
    m_viewers[i]->GetRenderer()->ResetCamera();
    m_planeWidgets[i]->SetInputData(m_converter->GetOutput());
    m_planeWidgets[i]->GetColorMap()->Update();
  }
  ResetViews();
}

void Visualize::ResetViews() {
  for (int i = 0; i < 3; i++) {
    m_viewers[i]->Reset();
    m_viewers[i]->GetResliceCursorWidget()->SetEnabled(1);
  }

  for (int i = 0; i < 3; i++) {
    vtkPlaneSource* ps = static_cast<vtkPlaneSource*>(
      m_planeWidgets[i]->GetPolyDataAlgorithm());
    ps->SetNormal(m_viewers[0]->GetResliceCursor()->GetPlane(i)->GetNormal());
    ps->SetCenter(m_viewers[0]->GetResliceCursor()->GetPlane(i)->GetOrigin());
    m_planeWidgets[i]->UpdatePlacement();
  }
  Render();
}

