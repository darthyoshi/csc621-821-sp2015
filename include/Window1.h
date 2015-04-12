#ifndef WINDOW1_H
#define WINDOW1_H

#include <QMainWindow>

#include <vtkSmartPointer.h>

class QAction;
class QMenu;
class QPlainTextEdit;

class vtkImageViewer2;
class vtkResliceImageViewer;

namespace Ui { class MainWindow; }

class Window1 : public QMainWindow {

  Q_OBJECT

  public:
    Window1(QWidget* parent = 0);
    ~Window1();

  private slots:
    int LoadDICOM();

  private:
    vtkImageViewer2* m_view;
    Ui::MainWindow* m_ui;
};

#endif
