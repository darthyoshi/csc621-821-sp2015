#ifndef REGWINDOW_H
#define REGWINDOW_H

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

#include "itkImage.h"
#include <itkImageSeriesReader.h>
#include <itkImageSeriesWriter.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageToVTKImageFilter.h>

#include "easylogging++.h"
#include "ui_RegWindow.h"

class QAction;
class QMenu;
class QPlainTextEdit;

class vtkImageViewer2;
class vtkResliceImageViewer;

typedef unsigned short InputPixel;
const int DIMS = 3;
typedef itk::Image<InputPixel, DIMS> InputImage;
typedef itk::ImageSeriesReader<InputImage> Reader;

namespace Ui { class RegMainWindow; }

class RegWindow : public QMainWindow {

  Q_OBJECT

  public:
    RegWindow(QWidget* parent = 0);
    ~RegWindow();

  private slots:
    void on_pushButton_OpenFixed_clicked();
    void on_pushButton_OpenMoving_clicked();

private:
    int LoadDICOMFixed();
    int LoadDICOMMoving();

    vtkImageViewer2* m_viewLeft;
    vtkImageViewer2* m_viewRight;
    Ui::RegMainWindow* m_ui;

    Reader::Pointer readerFixed;
    Reader::Pointer readerMoving;

};

#endif
