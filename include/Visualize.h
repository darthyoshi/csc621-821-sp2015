#ifndef VISUALIZE_H
#define VISUALIZE_H

#include <vector>
#include <type_traits>

#include <QObject>
#include <QMainWindow>
#include <QToolBox>
#include <QGridLayout>
#include <QStackedWidget>
#include <QtCore/qstatemachine.h>
#include <QtWidgets/qtoolbox.h>

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
      Visualize(QWidget* parent = 0);
      ~Visualize();

    public slots:

    protected:
      Ui::MainWindow* m_window;
      QToolBox* m_toolbox;

      int m_loadIdx;
      int m_registerIdx;
      int m_segmentIdx;
      int m_quantifyIdx;

      Loader* m_loader;
      Registrant* m_registrant;
      Segmentor* m_segmentor;
  };
}

#endif
