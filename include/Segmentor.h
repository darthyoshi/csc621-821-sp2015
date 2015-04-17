#ifndef SEGMENTOR_H
#define SEGMENTOR_H

#include <QWidget>

#include <vtkRenderWindow.h>

namespace vis {

  class Segmentor : public QWidget {

    Q_OBJECT

    public:
      Segmentor(QWidget* parent = 0);
      vtkRenderWindow* GetRenderWindow() {}

    public slots:
      void Thing() {}
  };

}

#endif
