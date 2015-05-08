#ifndef REGISTRATION_ITERATION_COMMAND_H
#define REGISTRATION_ITERATION_COMMAND_H

#include "itkObject.h"
#include "itkSmartPointer.h"

namespace vis {

  class RegistrationIterationCommand : public itk::Command {

    public:
      typedef RegistrationIterationCommand Self;
      typedef itk::Command Superclass;
      typedef itk::SmartPointer<Self> Pointer;
      typedef itk::VersorRigid3DTransformOptimizer Optimizer;

      itkNewMacro(Self)

    protected:
      RegistrationIterationCommand() {}

    public:

      void Execute(itk::Object* caller, const itk::EventObject& event) {
        Execute((const itk::Object*)caller, event);
      }

      void Execute(const itk::Object* object, const itk::EventObject& event) {
          const Optimizer* optimizer = static_cast<const Optimizer*>(object);

          if (!(itk::IterationEvent().CheckEvent(&event))) {
            return;
          }

          CLOG(INFO, "register") 
            << std::setw(15) << optimizer->GetCurrentIteration()
            << std::setw(15) << optimizer->GetValue() 
            << std::setw(15) << optimizer->GetCurrentPosition();
      }
  };
}

#endif
