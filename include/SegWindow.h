#ifndef SEGWINDOW_H
#define SEGWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QFileInfoList>
#include <QDir>
#include <iostream>

#include <vtkSmartPointer.h>
#include <vtkImageViewer2.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <itkImage.h>
#include <itkImageSeriesReader.h>
#include <itkImageSeriesWriter.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageToVTKImageFilter.h>

#include "easylogging++.h"
#include "ui_SegWindow.h"
#include "Segmentor.cpp"


class QAction;
class QMenu;
class QPlainTextEdit;

class vtkImageViewer2;
class vtkResliceImageViewer;

namespace Ui { class SegMainWindow; }

class SegWindow : public QMainWindow {

  Q_OBJECT

  public:
    SegWindow(QWidget* parent = 0);
    ~SegWindow();

  private slots:
    int LoadDICOM();
    int testSeg();

  private:
    vtkImageViewer2* m_view;
    Ui::SegMainWindow* m_ui;
};

#endif
