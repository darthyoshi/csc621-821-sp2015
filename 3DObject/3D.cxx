#include <vtkSmartPointer.h>
#include <vtkCamera.h>
#include <vtkFiniteDifferenceGradientEstimator.h>
#include <vtkImageClip.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkDICOMImageReader.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkColorTransferFunction.h>
#include <itkImageFileReader.h>
#include "itkImageToVTKImageFilter.h"
 // The Includes from somethine else

#include <itkImageFileReader.h>
#include "itkImageToVTKImageFilter.h"

#include <vtkSmartPointer.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVolumeProperty.h>
#include <vtkMatrix4x4.h>
#include <vtkAxesActor.h>
#include <iostream>
#include <string>
// Start Of the Includes from The other file..
#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
// headers needed for this example
#include <vtkImageViewer2.h>
#include <vtkDICOMImageReader.h>
#include <vtkInteractorStyleImage.h>
#include <vtkActor2D.h>
#include <vtkTextProperty.h>
#include <vtkTextMapper.h>
// Includes For the Series Reader...
#include "itkImageSeriesReader.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkLabelToRGBImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkLabelStatisticsImageFilter.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"


int main(int argc, char *argv[])
{
  if(argc < 2)
  {
    std::cerr << "Required arguments: DICOM directory" << std::endl;
    return EXIT_FAILURE;
  }

  std::string dirName = argv[1];

  /////////////////////////////////////////////////////////////////////////////////////////// REDNERING WINDOW COMPLETE //////////////////////////////////////////
  // Create the renderers, render window, and interactor
  vtkSmartPointer<vtkRenderWindow> renWin = 
    vtkSmartPointer<vtkRenderWindow>::New();

 // vtkSmartPointer<vtkRenderWindowInteractor> iren = 
  //  vtkSmartPointer<vtkRenderWindowInteractor>::New();

  ////////////////// ADDING A DIFFERNT WINDOW //////////////////////////////
 // vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
  vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();
  ren->SetBackground(0.5f, 0.5f, 1.0f);

  renWin->AddRenderer(ren);
  renWin->SetSize(1280, 1024);
  vtkSmartPointer<vtkRenderWindowInteractor> iren =
	  vtkSmartPointer<vtkRenderWindowInteractor>::New();

  /////////////////////////////////////////////
  //////// END OF THE NEW WINDOW! ////////////
  ////////////////////////////////////////////
  iren->SetRenderWindow(renWin);
  //vtkSmartPointer<vtkRenderer> ren =
   // vtkSmartPointer<vtkRenderer>::New();
 // renWin->AddRenderer(ren);

   
  // Read the data from a vtk file
  vtkSmartPointer<vtkDICOMImageReader> reader = 
    vtkSmartPointer<vtkDICOMImageReader>::New();
  reader->SetDirectoryName(dirName.c_str());
  reader->Update();


  //////////////////////////////////
  // After The reader Is Complete!
  ////////////////////////////////

  ///////////////////////////////////////////////////////////////////////////////// REDNERING WINDOW COMPLETE///////////////////////////










 
  /////////////////////The FOLLOWING IS THE EXAMPLE CODE FROM BEFORE REMOVING IT SO I CAN TRY SOMETHING ELSE/////////////////////////////
  // Create a transfer function mapping scalar value to opacity
  vtkSmartPointer<vtkPiecewiseFunction> oTFun = 
    vtkSmartPointer<vtkPiecewiseFunction>::New();
  oTFun->AddSegment(0, 1.0, 256, 0.1);

  ////ADDED THE OPACITY STUFF
  oTFun->AddPoint(0.0, 0.0);
  oTFun->AddPoint(80.0, 1.0);
  oTFun->AddPoint(80.1, 0.0);
  oTFun->AddPoint(255.0, 0.0);
  ///////////////////////////

 // oTFun->SetScalarOpacity(compositeOpacity);
 
  vtkSmartPointer<vtkColorTransferFunction> cTFun = 
    vtkSmartPointer<vtkColorTransferFunction>::New();

  // OLD POINTS
  //cTFun->AddRGBPoint(   0, 1.0, 1.0, 1.0 );
  //cTFun->AddRGBPoint( 255, 1.0, 1.0, 1.0 );

  // NEW Points
  cTFun->AddRGBPoint(0.0, 0.0, 0.0, 1.0);
  cTFun->AddRGBPoint(80.0, 1.0, 0.0, 0.0);
  cTFun->AddRGBPoint(255.0, 1.0, 1.0, 1.0);

  vtkSmartPointer<vtkVolumeProperty> property = 
    vtkSmartPointer<vtkVolumeProperty>::New();
  property->SetScalarOpacity(oTFun);
  property->SetColor(cTFun);
  property->SetInterpolationTypeToLinear();
 
  vtkSmartPointer<vtkFixedPointVolumeRayCastMapper> mapper = 
    vtkSmartPointer<vtkFixedPointVolumeRayCastMapper>::New();
  mapper->SetBlendModeToMaximumIntensity();
  mapper->SetInputConnection( reader->GetOutputPort() );
 
  vtkSmartPointer<vtkVolume> volume = 
    vtkSmartPointer<vtkVolume>::New();
  volume->SetMapper(mapper);
  volume->SetProperty(property);

  //////////////////////////////////// END OF THE ORIGINAL FILE!//////////////////////











  ///////////////////////////
  /// The Volume Is set now!
  //////////////////////////
  /*
  typedef itk::Image<unsigned char, 3> VisualizingImageType;
  typedef itk::ImageFileReader< VisualizingImageType >  ReaderType;

  VisualizingImageType::Pointer image = reader->GetOutput();

  vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
  vtkSmartPointer<vtkRenderer> ren1 = vtkSmartPointer<vtkRenderer>::New();
  ren1->SetBackground(0.5f, 0.5f, 1.0f);

  renWin->AddRenderer(ren1);
  renWin->SetSize(1280, 1024);
  vtkSmartPointer<vtkRenderWindowInteractor> iren =
	  vtkSmartPointer<vtkRenderWindowInteractor>::New();
  iren->SetRenderWindow(renWin);
  renWin->Render(); // make sure we have an OpenGL context.

  typedef itk::ImageToVTKImageFilter<VisualizingImageType> itkVtkConverter;
  itkVtkConverter::Pointer conv = itkVtkConverter::New();
  conv->SetInput(image);
  conv->Update();

  vtkSmartPointer<vtkImageData> image2 = vtkSmartPointer<vtkImageData>::New();
  image2->ShallowCopy(conv->GetOutput());
  //shallow copy is vtk's equivalent of disconnect pipeline

  vtkSmartPointer<vtkGPUVolumeRayCastMapper> volumeMapper =
	  vtkSmartPointer<vtkGPUVolumeRayCastMapper>::New();
  volumeMapper->SetInputData(image2);

  vtkSmartPointer<vtkVolumeProperty> volumeProperty =
	  vtkSmartPointer<vtkVolumeProperty>::New();

  vtkSmartPointer<vtkPiecewiseFunction> compositeOpacity =
	  vtkSmartPointer<vtkPiecewiseFunction>::New();
  compositeOpacity->AddPoint(0.0, 0.0);
  compositeOpacity->AddPoint(80.0, 1.0);
  compositeOpacity->AddPoint(80.1, 0.0);
  compositeOpacity->AddPoint(255.0, 0.0);
  volumeProperty->SetScalarOpacity(compositeOpacity);

  vtkSmartPointer<vtkColorTransferFunction> color =
	  vtkSmartPointer<vtkColorTransferFunction>::New();
  color->AddRGBPoint(0.0, 0.0, 0.0, 1.0);
  color->AddRGBPoint(40.0, 1.0, 0.0, 0.0);
  color->AddRGBPoint(255.0, 1.0, 1.0, 1.0);
  volumeProperty->SetColor(color);

  vtkSmartPointer<vtkVolume> volume =
	  vtkSmartPointer<vtkVolume>::New();
  volume->SetMapper(volumeMapper);
  volume->SetProperty(volumeProperty);

  //Here we take care of position and orientation
  //so that volume is in DICOM patient physical space
  VisualizingImageType::DirectionType d = image->GetDirection();
  vtkMatrix4x4 *mat = vtkMatrix4x4::New(); //start with identity matrix
  for (int i = 0; i<3; i++)
	  for (int k = 0; k<3; k++)
		  mat->SetElement(i, k, d(i, k));

  //counteract the built-in translation by origin
  VisualizingImageType::PointType origin = image->GetOrigin();
  volume->SetPosition(-origin[0], -origin[1], -origin[2]);

  //add translation to the user matrix
  for (int i = 0; i<3; i++)
	  mat->SetElement(i, 3, origin[i]);
  volume->SetUserMatrix(mat);

  //Add coordinate system axes, so we have a reference for position and orientation
  vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
  axes->SetTotalLength(250, 250, 250);
  axes->SetShaftTypeToCylinder();
  axes->SetCylinderRadius(0.01);
  ren1->AddActor(axes);

  ren1->AddVolume(volume);
  ren1->ResetCamera();

  renWin->Render();
  renWin->Render();
  renWin->Render();

  iren->Start();
  /// END OF THE OLD STUFF
  */

 
  ren->AddViewProp(volume);
  renWin->Render();
  iren->Start();
 
  return EXIT_SUCCESS;
}
