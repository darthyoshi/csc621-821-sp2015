#include "Visualize.h"

using namespace vis;

Visualize::Visualize(QWidget* parent) : QMainWindow(parent),
  m_window(new Ui::MainWindow())
{
  m_window->setupUi(this);

  QToolBox* toolbox = new QToolBox();
  QStackedWidget* stack = new QStackedWidget();

  m_window->sideBar->layout()->addWidget(toolbox);
  m_window->mainArea->layout()->addWidget(stack);

  m_loader = new Loader();
  int m_loadIdx = toolbox->addItem(m_loader->GetToolbox(), tr("Load"));
  stack->addWidget(m_loader->GetContent());

  m_registrant = new Registrant();
  int m_registerIdx = toolbox->addItem(m_registrant->GetToolbox(), tr("Register"));
  stack->addWidget(m_registrant->GetContent());

  m_segmentor = new Segmentor();
  int m_segmentIdx = toolbox->addItem(m_segmentor->GetToolbox(), tr("Segment"));
  stack->addWidget(m_segmentor->GetContent());

  m_quantifier = new Quantifier();
  int m_quantifyIdx = toolbox->addItem(m_quantifier->GetToolbox(), tr("Quantify"));
  stack->addWidget(m_quantifier->GetContent());

  toolbox->setItemEnabled(m_registerIdx, false);
  toolbox->setItemEnabled(m_segmentIdx, false);
  toolbox->setItemEnabled(m_quantifyIdx, false);

  this->showMaximized();

  // Connect sidebar toolbox to display the correct content widget when it is
  // selected.
  connect(toolbox, &QToolBox::currentChanged, stack, &QStackedWidget::setCurrentIndex);

  // Connect each stage signal to the appropriate slots on every concerned
  // stage.
  connect(m_loader, &Loader::SourceChanged, m_registrant, &Registrant::SetFixedSource);
  connect(m_loader, &Loader::SourceChanged, m_segmentor, &Segmentor::SetFixedImage);
  connect(m_registrant, &Registrant::RegistrationComplete, m_segmentor, &Segmentor::SetMovingImage);
  connect(m_segmentor, &Segmentor::SegmentationComplete, m_quantifier, &Quantifier::SetImage);

  // Enable appropriate stages when their inputs have become available.
  connect(m_loader, &Loader::SourceChanged, [=](BaseImage::Pointer b) {
      toolbox->setItemEnabled(m_registerIdx, true);
  });
  connect(m_registrant, &Registrant::RegistrationComplete, [=](BaseImage::Pointer) {
      toolbox->setItemEnabled(m_segmentIdx, true);
  });
  connect(m_segmentor, &Segmentor::SegmentationComplete, [=](BaseImage::Pointer b) {
      toolbox->setItemEnabled(m_quantifyIdx, true);
  });

  // Connect the window's exit action with a proper cleanup.
  connect(m_window->actionExit, SIGNAL(triggered()), this, SLOT(close()));
}

Visualize::~Visualize() {
  delete m_window;
}

