#ifndef VISUALIZE_IMAGE_DATA_H
#define VISUALIZE_IMAGE_DATA_H

#include <vtkImageData.h>

#include <itkImage.h>
#include <itkImageToVTKImageFilter.h>
#include <itkImageSource.h>

#include "Common.h"

namespace vis {

  typedef itk::Image<itk::RGBPixel<unsigned char>, 3> MergedImage;

  /**
   * VisualizeImageSource
   *
   * This class represents a custom image source used by Visualize to
   * hold various representations of the same image data in a
   * layered structure. It allows the user to display colored
   * overlays on top of a base image, but maintains the 3D structure
   * of the data so that it can be forwarded to vtkResliceImageViewer.
   */
  class VisualizeImageSource : public itk::ImageSource<MergedImage> {

    public:
      typedef VisualizeImageSource Self;
      typedef itk::ImageSource<MergedImage> Superclass;
      typedef itk::SmartPointer<Self> Pointer;

    public:
      itkNewMacro(Self)
      itkTypeMacro(VisualizeImageSource, ImageSource)

      void SetBaseImage(BaseImage::Pointer baseImage);

    protected:
      VisualizeImageSource();
      ~VisualizeImageSource();

      virtual void GenerateData();

    private:
      BaseImage::Pointer m_base;
  };
}
#endif
