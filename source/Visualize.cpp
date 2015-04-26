#include "Visualize.h"

using namespace vis;

Visualize::Visualize(QWidget* parent = 0) : QMainWindow(parent),
  m_window(new Ui::MainWindow()) 
{
  m_window->setupUi(this);

  QToolBox* toolbox = new QToolBox();
  QStackedWidget* stack = new QStackedWidget();

  m_window->sideBar->layout()->addWidget(toolbox);
  m_window->mainArea->layout()->addWidget(stack);

  // Master state machine for the application.
  m_loader = new Loader();
  toolbox->addItem(m_loader->GetToolbox(), tr("Load"));
  stack->addWidget(m_loader->GetContent());

  m_registrant = new Registrant();
  toolbox->addItem(m_registrant->GetToolbox(), tr("Register"));
  stack->addWidget(m_registrant->GetContent());

  m_segmentor = new Segmentor();
  toolbox->addItem(m_segmentor->GetToolbox(), tr("Segment"));
  stack->addWidget(m_segmentor->GetContent());

  this->showMaximized();

  connect(toolbox, &QToolBox::currentChanged, stack, &QStackedWidget::setCurrentIndex);
  connect(m_window->actionExit, SIGNAL(triggered()), this, SLOT(close()));
}

Visualize::~Visualize() {
  delete m_window; 
}

