#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>

#include <vtkSmartPointer.h>

class QAction;
class QMenu;
class QPlainTextEdit;

class vtkImageViewer2;
class vtkResliceImageViewer;

namespace Ui { class MainWindow; }

class Window : public QMainWindow {

  Q_OBJECT

  public:
    Window(QWidget* parent = 0);
    ~Window();

  private slots:
    int LoadDICOM();

  private:
    vtkImageViewer2* m_view;
    Ui::MainWindow* m_ui;
};

#endif
