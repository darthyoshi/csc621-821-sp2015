#ifndef BOX_WIDGET_CALLBACK_H
#define BOX_WIDGET_CALLBACK_H

#include "vtkBoxWidget2.h"
#include "vtkBoxRepresentation.h"
#include "vtkSmartVolumeMapper.h"
#include "vtkPolyData.h"
#include "vtkPlanes.h"
#include "vtkCommand.h"

namespace vis {
  class BoxWidgetCallback : public vtkCommand {
    public:
      static BoxWidgetCallback* New() { return new BoxWidgetCallback; }

      virtual void Execute(vtkObject* caller, unsigned long, void*) {
        vtkBoxWidget2* widget = reinterpret_cast<vtkBoxWidget2*>(caller);
        if (this->mapper) {
          vtkPlanes* planes = vtkPlanes::New();
          auto representation = static_cast<
            vtkBoxRepresentation*
          >(widget->GetRepresentation());
          representation->GetPlanes(planes);
          mapper->SetClippingPlanes(planes);
        }
      }

      vtkSmartVolumeMapper* mapper;
  };
}

#endif
