// Bootstrap the logger.
#include "easylogging++.h"
#define ELPP_STL_LOGGING 1
INITIALIZE_EASYLOGGINGPP

#include <QApplication>

#include "Window.h"
#include "Window1.h"

int main(int argc, char* argv[]) {
  // Configure the logger.
  el::Configurations conf;
  conf.setGlobally(el::ConfigurationType::Format, "[%logger] %level: %msg");
  conf.setGlobally(el::ConfigurationType::Filename, "/tmp/logs/visualize.log");
  el::Loggers::setDefaultConfigurations(conf, true);
  el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
  el::Loggers::addFlag(el::LoggingFlag::CreateLoggerAutomatically);
  el::Loggers::addFlag(el::LoggingFlag::AutoSpacing);

  QApplication app(argc, argv);
  app.setOrganizationName("Group Wind");
  app.setApplicationName("Visualization Example");

  Window window;
  window.showMaximized();
  window.show();

  Window1 windowOne;
  windowOne.showMaximized();
  windowOne.show();

  return app.exec();
};
