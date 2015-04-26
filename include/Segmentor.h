#ifndef SEGMENTOR_H
#define SEGMENTOR_H

#include <QWidget>

#include <vtkRenderWindow.h>

#include "Stage.h"

namespace vis {

  class Segmentor : public Stage {

    Q_OBJECT

    public:
      Segmentor();

      QWidget* GetContent();
      QWidget* GetToolbox();

    public slots:

  };

}

#endif
