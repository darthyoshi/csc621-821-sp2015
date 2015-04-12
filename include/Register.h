#ifndef Register_H
#define Register_H

#include <QMainWindow>

#include <vtkSmartPointer.h>

class QAction;
class QMenu;
class QPlainTextEdit;

class vtkImageViewer2;
class vtkResliceImageViewer;

namespace Ui { class MainWindow; }

class Register : public QMainWindow {

  Q_OBJECT

  public:
    Register(QWidget* parent = 0);
    ~Register();

  private slots:
    int LoadDICOM();
    int LoadDICOM2();

  private:
    vtkImageViewer2* m_view;
    Ui::MainWindow* m_ui;
};

#endif
