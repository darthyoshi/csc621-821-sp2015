/*
 * Adapted from VTK ReadDICOMSeries example. Operations and objects
 * required for cine view have been encapsulated in the CineViewer class.
 */
// some standard vtk headers
#include <vtkSmartPointer.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
// headers needed for this example
#include <vtkImageViewer2.h>
#include <vtkDICOMImageReader.h>
#include <vtkInteractorStyleImage.h>
#include "CineViewer.h"

// Define own interaction style
class newInteractorStyleImage : public vtkInteractorStyleImage {
public:
    static newInteractorStyleImage* New();
    vtkTypeMacro(newInteractorStyleImage, vtkInteractorStyleImage);

protected:
    CineViewer* _Viewer;

public:
    void setCineViewer(CineViewer* viewer) {
        _Viewer = viewer;
    }

    virtual void OnKeyDown() {
        std::string key = this->GetInteractor()->GetKeySym();
        if(key.compare("Up") == 0) {
            //cout << "Up arrow key was pressed." << endl;
            _Viewer->MoveSliceForward();
        }
        else if(key.compare("Down") == 0) {
            //cout << "Down arrow key was pressed." << endl;
            _Viewer->MoveSliceBackward();
        }
        // forward event
        vtkInteractorStyleImage::OnKeyDown();
    }


    virtual void OnMouseWheelForward() {
        //std::cout << "Scrolled mouse wheel forward." << std::endl;
        _Viewer->MoveSliceForward();
        // don't forward events, otherwise the image will be zoomed 
        // in case another interactorstyle is used (e.g. trackballstyle, ...)
        // vtkInteractorStyleImage::OnMouseWheelForward();
    }


    virtual void OnMouseWheelBackward() {
        //std::cout << "Scrolled mouse wheel backward." << std::endl;
        _Viewer->MoveSliceBackward();
        // don't forward events, otherwise the image will be zoomed 
        // in case another interactorstyle is used (e.g. trackballstyle, ...)
        // vtkInteractorStyleImage::OnMouseWheelBackward();
    }
};

vtkStandardNewMacro(newInteractorStyleImage);


int main(int argc, char* argv[]) {
   // Verify input arguments
    if(argc != 2) {
        std::cout << "Usage: " << argv[0]
            << " FolderName" << std::endl;
        return EXIT_FAILURE;
    }

    CineViewer* viewer = new CineViewer();
    viewer->Init(argv[1]);

    vtkSmartPointer<newInteractorStyleImage> myInteractorStyle = vtkSmartPointer<newInteractorStyleImage>::New();
    myInteractorStyle->setCineViewer(viewer);

    // create an interactor with our own style (inherit from vtkInteractorStyleImage)
    // in order to catch mousewheel and key events
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();

    viewer->GetImageViewer()->SetupInteractor(renderWindowInteractor);
    // make the interactor use our own interactorstyle
    // cause SetupInteractor() is defining it's own default interatorstyle 
    // this must be called after SetupInteractor()
    renderWindowInteractor->SetInteractorStyle(myInteractorStyle);

    // initialize rendering and interaction
    //myInteractorStyle->GetImageViewer()->GetRenderWindow()->SetSize(400, 300);
    //myInteractorStyle->GetImageViewer()->GetRenderer()->SetBackground(0.2, 0.3, 0.4);
    viewer->GetImageViewer()->Render();
    viewer->GetImageViewer()->GetRenderer()->ResetCamera();
    viewer->GetImageViewer()->Render();
    renderWindowInteractor->Start();

    delete viewer;
    return EXIT_SUCCESS;
}
