#ifndef SEGMENT_H
#define SEGMENT_H

#include <QMainWindow>

#include <vtkSmartPointer.h>

class QAction;
class QMenu;
class QPlainTextEdit;

class vtkImageViewer2;
class vtkResliceImageViewer;

namespace Ui { class MainWindow; }

class Segment : public QMainWindow {

  Q_OBJECT

  public:
    Segment(QWidget* parent = 0);
    ~Segment();

  private slots:
    int LoadDICOM();

  private:
    vtkImageViewer2* m_view;
    Ui::MainWindow* m_ui;
};

#endif
