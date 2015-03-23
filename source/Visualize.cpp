// Bootstrap the logger.
#include "easylogging++.h"
#define ELPP_STL_LOGGING 1
INITIALIZE_EASYLOGGINGPP

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkRescaleIntensityImageFilter.h>

#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRenderer.h>
#include <vtkImageMapper.h>
#include <vtkActor2D.h>
#include <vtkImageSlice.h>

#include "DICOMSeries.h"

int main(int argc, char* argv[]) {
  // Configure the logger.
  el::Configurations conf;
  conf.setGlobally(el::ConfigurationType::Format, "[%logger] %level: %msg");
  conf.setGlobally(el::ConfigurationType::Filename, "/tmp/logs/visualize.log");
  el::Loggers::setDefaultConfigurations(conf, true);
  el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
  el::Loggers::addFlag(el::LoggingFlag::CreateLoggerAutomatically);
  el::Loggers::addFlag(el::LoggingFlag::AutoSpacing);

  DICOMSeries dcms(argv[1]);
  dcms.Load();

  auto window = vtkSmartPointer<vtkRenderWindow>::New();
  auto renderer = vtkSmartPointer<vtkRenderer>::New();
  renderer->SetBackground(0.5f, 0.5f, 1.0f);

  window->AddRenderer(renderer);
  window->SetSize(1280, 1024);

  auto style = vtkSmartPointer<vtkInteractorStyleImage>::New();
  auto interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  interactor->SetInteractorStyle(style);
  interactor->SetRenderWindow(window);

  
  // Map the image to the surface.
  auto image = dcms.GetOutput();
  auto mapper = vtkSmartPointer<vtkImageMapper>::New();
  mapper->SetInputData(image);
  mapper->SetColorWindow(255);
  mapper->SetColorLevel(127.5);

  auto actor = vtkSmartPointer<vtkActor2D>::New();
  actor->SetMapper(mapper);
  actor->SetPosition(20, 20);

  renderer->AddActor2D(actor);

  // Verify context.
  window->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}

