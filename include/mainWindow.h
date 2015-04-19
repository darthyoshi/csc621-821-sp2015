#ifndef MAIN_H
#define MAIN_H

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
#include "ui_MainWindow.h"

#include "Segment.h"

class QAction;
class QMenu;
class QPlainTextEdit;

class vtkImageViewer2;
class vtkResliceImageViewer;

typedef unsigned short InputPixel;
const int DIMS = 3;
typedef itk::Image<InputPixel, DIMS> InputImage;
typedef itk::ImageSeriesReader<InputImage> Reader;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_SelectFixedDir_clicked();

    void on_pushButton_SelectMovingDir_clicked();

    void on_pushButton_Register_clicked();

    void on_pushButton_SelectSegDir_clicked();

    void on_pushButton_Segment_clicked();

private:
    Ui::MainWindow *ui;

    int LoadDICOMFixed();
    int LoadDICOMMoving();
    int LoadDICOMSeg();

    vtkImageViewer2* m_viewLeft;
    vtkImageViewer2* m_viewRight;
    vtkImageViewer2* m_viewSeg;

    Reader::Pointer readerFixed;
    Reader::Pointer readerMoving;
    Reader::Pointer readerSeg;
};

#endif // MAIN_H
