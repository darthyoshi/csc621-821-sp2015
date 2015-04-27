#include "itkImage.h"
#include "itkRGBPixel.h"

namespace vis {
  /**
   * We use `signed short` as our base pixel type because it is the usual
   * type assigned to CT and X-ray image sources.
   */
  typedef itk::RGBPixel<signed short> BasePixel;
  typedef itk::Image<BasePixel, 3> BaseImage;
}