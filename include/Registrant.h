#ifndef REGISTRANT_H
#define REGISTRANT_H

#include <QWidget>

#include <vtkRenderWindow.h>

#include "itkImageSeriesReader.h"

#include "Stage.h"
#include "Common.h"

namespace vis {

  class Registrant : public Stage {

    Q_OBJECT

    typedef itk::ImageSeriesReader<BaseImage> Reader;

    public:
      Registrant();

      QWidget* GetToolbox();
      QWidget* GetContent();

    signals:
      void RegistrationComplete();
  };
}
#endif
