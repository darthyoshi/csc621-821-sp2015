#ifndef BOX_WIDGET_CALLBACK_H
#define BOX_WIDGET_CALLBACK_H

#include "vtkBoxWidget.h"
#include "vtkPlanes.h"
#include "vtkCommand.h"

namespace vis {
  class BoxWidgetCallback : public vtkCommand {
    public:
      static BoxWidgetCallback* New() { return new BoxWidgetCallback; }

      virtual void Execute(vtkObject* caller, unsigned long, void*) {
        vtkBoxWidget* widget = reinterpret_cast<vtkBoxWidget*>(caller);
        if (this->mapper) {
          vtkPlanes* planes = vtkPlanes::New();
          widget->GetPlanes(planes);
          this->mapper->SetClippingPlanes(planes);
          planes->Delete();
        }
      }

      vtkSmartVolumeMapper* mapper;
  };
}

#endif
