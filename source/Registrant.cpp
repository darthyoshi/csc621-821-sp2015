#include "Registrant.h"

using namespace vis;

Registrant::Registrant() : Stage() {}

QWidget* Registrant::GetToolbox() {
  return new QWidget();
}

QWidget* Registrant::GetContent() {
  return new QWidget();
}
