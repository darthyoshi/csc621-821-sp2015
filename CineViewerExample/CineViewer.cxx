/**
 * @file CineViewer.cxx
 * @author Kay Choi
 * @description Implementation of CineViewer class.
 */

#include <vtkSmartPointer.h>
#include <vtkObjectFactory.h>
#include <vtkImageViewer2.h>
#include <vtkDICOMImageReader.h>
#include "CineViewer.h"

/**
 * Initializes the CineViewer.
 * @param dirName the DICOM image directory path
 */
void CineViewer::Init(char* dirName) {
    //read all DICOM files in specified directory
    vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
    reader->SetDirectoryName(dirName);
    reader->Update();

    //visualize DICOM files
    _ImageViewer = vtkSmartPointer<vtkImageViewer2>::New();
    _ImageViewer->SetInputConnection(reader->GetOutputPort());

    SetSlices();
}

/**
 * Initializes the CineViewer.
 * @param reader a image reader with the desired DICOM images
 */
void CineViewer::Init(vtkDICOMImageReader* reader) {
    _ImageViewer = vtkSmartPointer<vtkImageViewer2>::New();
    _ImageViewer->SetInputConnection(reader->GetOutputPort());

    SetSlices();
}

/**
 * Initializes the CineViewer.
 * @param viewer an image viewer with the desired DICOM images
 */
void CineViewer::Init(vtkImageViewer2* viewer) {
    _ImageViewer = viewer;

    SetSlices();
}

/**
 * Sets the private slice variables.
 */
void CineViewer::SetSlices() {
    _MinSlice = _ImageViewer->GetSliceMin();
    _MaxSlice = _ImageViewer->GetSliceMax();
    _Slice = _MinSlice;

    //cout << "Slicer: Min = " << _MinSlice << ", Max = " << _MaxSlice << std::endl;
}

/**
 * @return the associated vtkImageViewer2
 */
vtkImageViewer2 *CineViewer::GetImageViewer() {
    return _ImageViewer;
}

/**
 * @return the current slice number
 */
int CineViewer::GetCurrentSlice() {
    return _Slice;
}

/**
 * @return the first slice number
 */
int CineViewer::GetMinSlice() {
    return _MinSlice;
}

/**
 * @return the last slice number
 */
int CineViewer::GetMaxSlice() {
    return _MaxSlice;
}

/**
 * Advances the viewer by one slice.
 */
void CineViewer::MoveSliceForward() {
    if(_Slice < _MaxSlice) {
        _Slice += 1;
        _ImageViewer->SetSlice(_Slice);
        _ImageViewer->Render();
        //cout << "MoveSliceForward::Slice = " << _Slice << std::endl;
    }
}

/**
 * Reverses the viewer by one slice.
 */
void CineViewer::MoveSliceBackward() {
    if(_Slice > _MinSlice) {
        _Slice -= 1;
        _ImageViewer->SetSlice(_Slice);
        _ImageViewer->Render();
        //cout << "MoveSliceBackward::Slice = " << _Slice << std::endl;
    }
}
