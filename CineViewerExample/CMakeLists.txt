
cmake_minimum_required(VERSION 2.8)

PROJECT(CineViewerExample)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(CineViewerExample MACOSX_BUNDLE CineViewerExample CineViewer.cxx)

if(VTK_LIBRARIES)
  target_link_libraries(CineViewerExample ${VTK_LIBRARIES})
else()
  target_link_libraries(CineViewerExample vtkHybrid vtkWidgets)
endif()
