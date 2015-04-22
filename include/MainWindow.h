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
#include <itkImageToVTKImageFilter.h>
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include <vector>
#include "itksys/SystemTools.hxx"

//#include "itkSubtractImageFilter.h"
//#include "itkThresholdImageFilter.h"

#include "easylogging++.h"
#include "ui_MainWindow.h"

class QAction;
class QMenu;
class QPlainTextEdit;

class vtkImageViewer2;
class vtkResliceImageViewer;

typedef unsigned char InputPixel;
const int DIMSIN = 3;
typedef itk::Image<InputPixel, DIMSIN> InputImage;
const int DIMSOUT = 2;
typedef itk::Image<InputPixel, DIMSOUT> OutputImage;

typedef itk::ImageSeriesReader<InputImage> Reader;
typedef itk::ImageSeriesWriter<InputImage, OutputImage> Writer;
typedef itk::ImageToVTKImageFilter<InputImage> Connector;

typedef itk::GDCMImageIO ImageIOType;
typedef itk::GDCMSeriesFileNames NamesGeneratorType;

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
    void on_pushButton_SelectRegFixedDir_clicked();

    void on_pushButton_SelectRegMoving1Dir_clicked();

    void on_pushButton_SelectRegMoving2Dir_clicked();

    void on_pushButton_Register_clicked();

    void on_pushButton_SelectSeg1Dir_clicked();

    void on_pushButton_SelectSeg2Dir_clicked();

    void on_pushButton_Segment_clicked();

    void on_pushButton_SelectCineDir_clicked();

    void on_pushButton_SelectMIPDir_clicked();

private:
    Ui::MainWindow *ui;

    const int tabIdxReg = 0;
    const int tabIdxSeg = 1;
    const int tabIdxCine = 2;
    const int tabIdxMIP = 3;

    void InitializeRenderWindow(vtkImageViewer2*, QVTKWidget*);
    void setTab (int tab);
    int LoadDICOM (
            Reader::Pointer reader,
            vtkImageViewer2 *m_view,
            QVTKWidget *qVTKWidget
            );
    int convertITKToVTK (
            const itk::Image<unsigned char, 3> * imageITK,
            vtkImageViewer2 *m_view,
            QVTKWidget *qVTKWidget
            );

    vtkImageViewer2* m_viewRegFixed;
    vtkImageViewer2* m_viewRegMoving1;
    vtkImageViewer2* m_viewRegMoving2;
    vtkImageViewer2* m_viewSeg1;
    vtkImageViewer2* m_viewSeg2;
    vtkImageViewer2* m_viewCine;
    vtkImageViewer2* m_viewMIP;

    Reader::Pointer readerRegFixed;
    Reader::Pointer readerRegMoving1;
    Reader::Pointer readerRegMoving2;
    Reader::Pointer readerSeg1;
    Reader::Pointer readerSeg2;
    Reader::Pointer readerCine;
};

#endif // MAIN_H
