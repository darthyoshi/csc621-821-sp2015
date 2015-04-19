// Bootstrap the logger.
#include "easylogging++.h"
#define ELPP_STL_LOGGING 1
INITIALIZE_EASYLOGGINGPP

#include <QApplication>

#include "RegWindow.h"
#include "SegWindow.h"

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

  RegWindow regWindow;
  regWindow.showMaximized();
  regWindow.show();

  SegWindow segWindow;
  segWindow.showMaximized();
  segWindow.show();

  return app.exec();
};
