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

    void on_pushButton_SelectCineDir_clicked();

    void on_pushButton_SelectMIPDir_clicked();

    void InitializeRenderWindow(vtkImageViewer2*, QVTKWidget*);

private:
    Ui::MainWindow *ui;

    int LoadDICOM (
            Reader::Pointer reader,
            vtkImageViewer2 *m_view,
            QVTKWidget *qVTKWidget);

    vtkImageViewer2* m_viewLeft;
    vtkImageViewer2* m_viewRight;
    vtkImageViewer2* m_viewSeg;
    vtkImageViewer2* m_viewCine;
    vtkImageViewer2* m_viewMIP;

    Reader::Pointer readerFixed;
    Reader::Pointer readerMoving;
    Reader::Pointer readerSeg;
    Reader::Pointer readerCine;
};

#endif // MAIN_H
