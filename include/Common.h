#ifndef __COMMON__
#define __COMMON__

#include "itkImage.h"
#include "itkRGBPixel.h"

namespace vis {
  /**
   * We use `signed short` as our base pixel type because it is the usual
   * type assigned to CT and X-ray image sources.
   */
  const   unsigned int    Dimension = 3;
  // typedef itk::RGBPixel<signed short> BasePixel;
  typedef signed short BasePixel;
  typedef itk::Image<BasePixel, 3> BaseImage;
}

#endif
