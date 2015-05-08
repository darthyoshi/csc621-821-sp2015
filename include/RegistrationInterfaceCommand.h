#ifndef REGISTRATION_INTERFACE_COMMAND_H
#define REGISTRATION_INTERFACE_COMMAND_H

#include "itkCommand.h"
#include "itkSmartPointer.h"

namespace vis {

  template <typename Registration>
  class RegistrationInterfaceCommand : public itk::Command {

    public:
      typedef RegistrationInterfaceCommand Self;
      typedef itk::Command Superclass;
      typedef itk::SmartPointer<Self> Pointer;
      typedef itk::VersorRigid3DTransformOptimizer Optimizer;

      itkNewMacro(Self)
      itkGetMacro(MaxStep, double)
      itkSetMacro(MaxStep, double)
      itkGetMacro(MinStep, double)
      itkSetMacro(MinStep, double)
      itkGetMacro(MaxStepReductionFactor, double)
      itkSetMacro(MaxStepReductionFactor, double)
      itkGetMacro(MinStepReductionFactor, double)
      itkSetMacro(MinStepReductionFactor, double)

    protected:
      RegistrationInterfaceCommand() {}

      double m_MaxStep = 0.2;
      double m_MinStep = 0.0001;
      double m_MaxStepReductionFactor = 1.0;
      double m_MinStepReductionFactor = 1.0;

    public:
      void Execute(const itk::Object* object, const itk::EventObject& event) {
        Execute(const_cast<itk::Object*>(object), event);
      }

      void Execute(itk::Object* object, const itk::EventObject& event) {
        if (!(itk::IterationEvent().CheckEvent(&event))) {
          return;
        }

        const Registration* registration = 
          static_cast<const Registration*>(object);

        if(!registration) { return; }

        Optimizer::Pointer optimizer = 
          static_cast<Optimizer::Pointer>(registration->GetModifiableOptimizer());

        CLOG(INFO, "register") << "-------------------------------------";
        CLOG(INFO, "register") << "MultiResolution Level : "
                  << registration->GetCurrentLevel();

        if (registration->GetCurrentLevel() == 0 ) {
          optimizer->SetMaximumStepLength(m_MaxStep);
          optimizer->SetMinimumStepLength(m_MinStep);
        } else {
          auto reducedMax = 
            optimizer->GetMaximumStepLength() / m_MaxStepReductionFactor;
          auto reducedMin = 
            optimizer->GetMinimumStepLength() / m_MinStepReductionFactor;

          optimizer->SetMaximumStepLength(reducedMax);
          optimizer->SetMinimumStepLength(reducedMin);
        }
      }
  };
}
#endif
