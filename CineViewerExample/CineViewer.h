/**
 * @file CineViewer.h
 * @author Kay Choi
 * @description Definition of CineViewer class.
 */
 
 class CineViewer {
protected:
    vtkSmartPointer<vtkImageViewer2> _ImageViewer;
    int _Slice;
    int _MinSlice;
    int _MaxSlice;

public:
    /**
     * Initializes the CineViewer.
     * @param dirName the DICOM image directory path
     */
    void Init(char* dirName);

    /**
     * Initializes the CineViewer.
     * @param reader a image reader with the desired DICOM images
     */
    void Init(vtkDICOMImageReader* reader);

    /**
     * Initializes the CineViewer.
     * @param viewer an image viewer with the desired DICOM images
     */
    void Init(vtkImageViewer2* viewer);

    /**
     * @return the associated vtkImageViewer2
     */
    vtkImageViewer2 *GetImageViewer();

    /**
     * @return the current slice number
     */
    int GetCurrentSlice();

    /**
     * @return the first slice number
     */
    int GetMinSlice();

    /**
     * @return the last slice number
     */
    int GetMaxSlice();

    /**
     * Advances the viewer by one slice.
     */
    void MoveSliceForward();

    /**
     * Reverses the viewer by one slice.
     */
    void MoveSliceBackward();

private:
    void SetSlices();
};
