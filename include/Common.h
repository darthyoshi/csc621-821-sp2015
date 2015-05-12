#ifndef COMMON_H
#define COMMON_H

#include "itkImage.h"
#include "itkRGBPixel.h"
#include "itkRGBAPixel.h"

namespace vis {
  /**
   * We use `signed short` as our base pixel type because it is the usual
   * type assigned to CT and X-ray image sources.
   */
  typedef signed short BasePixel;
  typedef itk::Image<BasePixel, 3> BaseImage;

  typedef itk::RGBPixel<unsigned char> ColorPixel;
  typedef itk::Image<ColorPixel, 3> ColorImage;

  typedef unsigned int LabelPixel;
  typedef itk::Image<LabelPixel, 3> LabelImage;
}

#endif
