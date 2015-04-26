#include "Segmentor.h"

using namespace vis;

Segmentor::Segmentor() : Stage() {}

QWidget* Segmentor::GetToolbox() {
  return new QWidget();
}

QWidget* Segmentor::GetContent() {
  return new QWidget();
}

