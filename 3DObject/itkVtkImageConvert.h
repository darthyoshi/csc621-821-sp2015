
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
 // Start Of the Includes from The Other File..
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
// needed to easily convert int to std::string
#include <sstream>


using namespace std;

//////////////////////////////////////////////////////////
// Start Of the Funcitons -- May not need these for later
////////////////////////////////////////////////////////
class StatusMessage
{
public:
	static std::string Format(int slice, int maxSlice)
	{
		std::stringstream tmp;
		tmp << "Slice Number  " << slice + 1 << "/" << maxSlice + 1;
		return tmp.str();
	}
};


// Define own interaction style
class myVtkInteractorStyleImage : public vtkInteractorStyleImage
{
public:
	static myVtkInteractorStyleImage* New();
	vtkTypeMacro(myVtkInteractorStyleImage, vtkInteractorStyleImage);

protected:
	vtkImageViewer2* _ImageViewer;
	vtkTextMapper* _StatusMapper;
	int _Slice;
	int _MinSlice;
	int _MaxSlice;

public:
	void SetImageViewer(vtkImageViewer2* imageViewer) {
		_ImageViewer = imageViewer;
		_MinSlice = imageViewer->GetSliceMin();
		_MaxSlice = imageViewer->GetSliceMax();
		_Slice = _MinSlice;
		cout << "Slicer: Min = " << _MinSlice << ", Max = " << _MaxSlice << std::endl;
	}

	void SetStatusMapper(vtkTextMapper* statusMapper) {
		_StatusMapper = statusMapper;
	}


protected:
	void MoveSliceForward()
	{
		if (_Slice < _MaxSlice) {
			_Slice += 1;
			cout << "MoveSliceForward::Slice = " << _Slice << std::endl;
			_ImageViewer->SetSlice(_Slice);
			std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
			_StatusMapper->SetInput(msg.c_str());
			_ImageViewer->Render();
		}
	}

	void MoveSliceBackward()
	{
		if (_Slice > _MinSlice) {
			_Slice -= 1;
			cout << "MoveSliceBackward::Slice = " << _Slice << std::endl;
			_ImageViewer->SetSlice(_Slice);
			std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
			_StatusMapper->SetInput(msg.c_str());
			_ImageViewer->Render();
		}
	}


	virtual void OnKeyDown()
	{
		std::string key = this->GetInteractor()->GetKeySym();
		if (key.compare("Up") == 0) {
			//cout << "Up arrow key was pressed." << endl;
			MoveSliceForward();
		}
		else if (key.compare("Down") == 0) {
			//cout << "Down arrow key was pressed." << endl;
			MoveSliceBackward();
		}
		// forward event
		vtkInteractorStyleImage::OnKeyDown();
	}


	virtual void OnMouseWheelForward()
	{
		//std::cout << "Scrolled mouse wheel forward." << std::endl;
		MoveSliceForward();
		// don't forward events, otherwise the image will be zoomed 
		// in case another interactorstyle is used (e.g. trackballstyle, ...)
		// vtkInteractorStyleImage::OnMouseWheelForward();
	}


	virtual void OnMouseWheelBackward()
	{
		//std::cout << "Scrolled mouse wheel backward." << std::endl;
		if (_Slice > _MinSlice) {
			MoveSliceBackward();
		}
		// don't forward events, otherwise the image will be zoomed 
		// in case another interactorstyle is used (e.g. trackballstyle, ...)
		// vtkInteractorStyleImage::OnMouseWheelBackward();
	}
};

vtkStandardNewMacro(myVtkInteractorStyleImage);


int main(int argc, char *argv[])
{

	
	//string hello;
	//cout << "Enter The File Name: ";
	//cin >> hello;



	//////////////////////////////////////////////////////////////////
	// Start Of the Dicom Reader... Going to Correctly Read the File
	////////////////////////////////////////////////////////////////
	typedef itk::Image<unsigned char, 3> VisualizingImageType;
	typedef itk::ImageFileReader<VisualizingImageType>  ReaderType;

	// Verify input arguments
	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0]
			<< " FolderName" << std::endl;
		return EXIT_FAILURE;
	}

	std::string folder = argv[1];
	//std::string folder = "C:\\VTK\\vtkdata-5.8.0\\Data\\DicomTestImages";

	// Read all the DICOM files in the specified directory.
	vtkSmartPointer<vtkDICOMImageReader> reader =
		vtkSmartPointer<vtkDICOMImageReader>::New();

	reader->SetDirectoryName(folder.c_str());
	reader->Update();

	////////////////////////////////////////////////
	/////// Start Of thE 3D creation Of the object
	//////////////////////////////////////////////

    //typedef itk::Image<unsigned char, 3> VisualizingImageType;
    //typedef itk::ImageFileReader< VisualizingImageType >  ReaderType;
    //ReaderType::Pointer reader = ReaderType::New();
    //reader->SetFileName( argv[1] );
	//reader->SetFileName(hello); // Changed This to Ask For A FileName
    //reader->Update();
    VisualizingImageType::Pointer image = reader->GetOutput();
 
    vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
    vtkSmartPointer<vtkRenderer> ren1 = vtkSmartPointer<vtkRenderer>::New();
    ren1->SetBackground(0.5f,0.5f,1.0f);
 
    renWin->AddRenderer(ren1);
    renWin->SetSize(1280,1024);
    vtkSmartPointer<vtkRenderWindowInteractor> iren = 
        vtkSmartPointer<vtkRenderWindowInteractor>::New();
    iren->SetRenderWindow(renWin);
    renWin->Render(); // make sure we have an OpenGL context.
 
    typedef itk::ImageToVTKImageFilter<VisualizingImageType> itkVtkConverter;
    itkVtkConverter::Pointer conv=itkVtkConverter::New();
    conv->SetInput(image);
    conv->Update();
 
    vtkSmartPointer<vtkImageData> image2=vtkSmartPointer<vtkImageData>::New();
    image2->ShallowCopy(conv->GetOutput());
    //shallow copy is vtk's equivalent of disconnect pipeline
 
    vtkSmartPointer<vtkGPUVolumeRayCastMapper> volumeMapper = 
        vtkSmartPointer<vtkGPUVolumeRayCastMapper>::New();
    volumeMapper->SetInputData(image2);
 
    vtkSmartPointer<vtkVolumeProperty> volumeProperty = 
    vtkSmartPointer<vtkVolumeProperty>::New();
 
    vtkSmartPointer<vtkPiecewiseFunction> compositeOpacity = 
    vtkSmartPointer<vtkPiecewiseFunction>::New();
    compositeOpacity->AddPoint(0.0,0.0);
    compositeOpacity->AddPoint(80.0,1.0);
    compositeOpacity->AddPoint(80.1,0.0);
    compositeOpacity->AddPoint(255.0,0.0);
    volumeProperty->SetScalarOpacity(compositeOpacity);
 
    vtkSmartPointer<vtkColorTransferFunction> color = 
    vtkSmartPointer<vtkColorTransferFunction>::New();
    color->AddRGBPoint(0.0  ,0.0,0.0,1.0);
    color->AddRGBPoint(40.0  ,1.0,0.0,0.0);
    color->AddRGBPoint(255.0,1.0,1.0,1.0);
    volumeProperty->SetColor(color);
 
    vtkSmartPointer<vtkVolume> volume = 
    vtkSmartPointer<vtkVolume>::New();
    volume->SetMapper(volumeMapper);
    volume->SetProperty(volumeProperty);
 
    //Here we take care of position and orientation
    //so that volume is in DICOM patient physical space
    VisualizingImageType::DirectionType d=image->GetDirection();
    vtkMatrix4x4 *mat=vtkMatrix4x4::New(); //start with identity matrix
    for (int i=0; i<3; i++)
        for (int k=0; k<3; k++)
            mat->SetElement(i,k, d(i,k));
 
    //counteract the built-in translation by origin
    VisualizingImageType::PointType origin=image->GetOrigin();
    volume->SetPosition(-origin[0], -origin[1], -origin[2]);
 
    //add translation to the user matrix
    for (int i=0; i<3; i++)
        mat->SetElement(i,3, origin[i]);
    volume->SetUserMatrix(mat);
 
    //Add coordinate system axes, so we have a reference for position and orientation
    vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
    axes->SetTotalLength(250,250,250);
    axes->SetShaftTypeToCylinder();
    axes->SetCylinderRadius(0.01);
    ren1->AddActor(axes);
 
    ren1->AddVolume( volume ); 
    ren1->ResetCamera();
 
    renWin->Render();
    renWin->Render();
    renWin->Render();
 
    iren->Start();
 
    return EXIT_SUCCESS;

}