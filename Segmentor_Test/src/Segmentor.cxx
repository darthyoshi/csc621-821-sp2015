#include "itkSubtractImageFilter.h"
#include "itkThresholdImageFilter.h"
#include "itkImage.h"



template< typename ImageType >
class Segmentor {

    typedef itk::SubtractImageFilter<ImageType,ImageType,ImageType > mySubtractFilterType;
    typedef itk::ThresholdImageFilter <ImageType>
    myThresholdImageFilterType;

    
    
private:
    typename mySubtractFilterType::Pointer subtractFilter;
    typename myThresholdImageFilterType::Pointer thresholdFilter;
    int lowerThreshold, upperThreshold;

    
public:
    
    Segmentor() {
        subtractFilter = mySubtractFilterType::New();
        thresholdFilter = myThresholdImageFilterType::New();
    }
    
    void SetInputs( ImageType *input1, ImageType *input2) {
        subtractFilter->SetInput1( input1 );
        subtractFilter->SetInput2( input2 );
    }

    void SetThreshold(int lowerThreshold, int upperThreshold) {
        thresholdFilter->ThresholdOutside(lowerThreshold, upperThreshold);
        thresholdFilter->SetOutsideValue(0);
    }
    
    ImageType *GetOutput() {
        thresholdFilter->SetInput(subtractFilter->GetOutput());
        return thresholdFilter->GetOutput();
    }
    
};