
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkRescaleIntensityImageFilter.h"

#include "QuickView.h"

typedef itk::Image<unsigned char, 2> ImageType;

static void GenerateImage(ImageType* const image);

int main(int argc, char* argv[]) {
  ImageType::Pointer image = ImageType::New();
  GenerateImage(image);

  QuickView viewer;
  viewer.AddImage(image.GetPointer());
  viewer.Visualize();

  return EXIT_SUCCESS;
}

void GenerateImage(ImageType* const image) {
  ImageType::IndexType corner = {{0, 0}};
  ImageType::SizeType size;

  size[0] = 200;
  size[1] = 300;

  ImageType::RegionType region(corner, size);
  image->SetRegions(region);
  image->Allocate();

  for (unsigned int r = 40; r < 100; r++) {
    for (unsigned int c = 40;  c < 100; c++) {
      ImageType::IndexType pixelIndex {r, c};
      image->SetPixel(pixelIndex, 15);
    }
  }
}


