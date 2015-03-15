class CineViewer {
protected:
    vtkSmartPointer<vtkImageViewer2> _ImageViewer;
    int _Slice;
    int _MinSlice;
    int _MaxSlice;

public:
    void Init(char* dirName);

    vtkImageViewer2 *GetImageViewer();

    int GetCurrentSlice();

    int GetMinSlice();

    int GetMaxSlice();

    void MoveSliceForward();

    void MoveSliceBackward();
};
