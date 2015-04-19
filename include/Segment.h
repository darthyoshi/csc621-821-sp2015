#ifndef Segment_H
#define Segment_H

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

#include "itkSubtractImageFilter.h"
#include "itkThresholdImageFilter.h"

class vtkImageViewer2;
class vtkResliceImageViewer;

class Segment {

  public:
    Segment();
    int testSeg();

  private:
};

#endif
