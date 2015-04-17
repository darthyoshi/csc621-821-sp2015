#ifndef REGISTRANT_H
#define REGISTRANT_H

#include <QWidget>

#include <vtkRenderWindow.h>

#include "itkImageSeriesReader.h"

#include "Loader.h"
#include "Common.h"

namespace vis {
  class Registrant : public QWidget {

    Q_OBJECT

    typedef itk::ImageSeriesReader<BaseImage> Reader;

    public:
      Registrant(QWidget* parent = 0);
      vtkRenderWindow* GetRenderWindow() {}

    signals:
      void RegistrationComplete();
  };
}
#endif
