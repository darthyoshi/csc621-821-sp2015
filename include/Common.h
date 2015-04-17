#include "itkImage.h"
#include "itkRGBPixel.h"

namespace vis {
  typedef unsigned int BasePixel;
  typedef itk::Image<BasePixel, 3> BaseImage;
  typedef itk::RGBPixel<unsigned int> LevelSetPixel;
  typedef itk::Image<LevelSetPixel> LevelSetImage;
}
