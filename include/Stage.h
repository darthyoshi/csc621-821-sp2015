#ifndef VISUALIZE_STAGE_H
#define VISUALIZE_STAGE_H

#include <QtWidgets/qwidget.h>
#include <QObject>

#include <vtkRenderWindow.h>

namespace vis {

  class Stage : public QObject {

    public:
      virtual QWidget* GetToolbox() = 0;
      virtual QWidget* GetContent() = 0;

    protected:
      virtual void BuildToolbox() {};
      virtual void BuildContent() {};
  };

}
#endif //VISUALIZE_STAGE_H
