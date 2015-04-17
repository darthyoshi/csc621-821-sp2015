#ifndef LOADER_H
#define LOADER_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QSlider>
#include <QGroupBox>
#include <QPushButton>
#include <QAbstractState>
#include <QFontMetrics>
#include <QFileDialog>
#include <string>

#include "itkImage.h"
#include "itkImageSeriesReader.h"
#include "itkImageToVTKImageFilter.h"

#include "easylogging++.h"

namespace vis {

  class Loader : public QWidget {

    Q_OBJECT

    public:
      typedef itk::Image<itk::RGBPixel<unsigned char>, 3> Image;
      typedef itk::ImageSeriesReader<Image> Reader;
      typedef itk::ImageToVTKImageFilter<Image> Converter;

    public:
      Loader(QWidget* parent = 0);

    signals:
      void SourceChanged(Reader::Pointer source);

    public slots:
      void LoadDICOMSource();

    private:
      Reader::Pointer m_source;
      QLabel* m_UIDLabel;
      QLabel* m_slicesLabel;
  };
}

#endif
