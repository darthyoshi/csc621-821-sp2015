cmake_minimum_required(VERSION 2.8)
 
project(visualize)
 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fpermissive")
set(EXECUTABLE_OUTPUT_PATH ${visualize_BINARY_DIR}/bin)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
if (ITKVtkGlue_LOADED)
find_package(VTK COMPONENTS
    vtkInteractionImage
    vtkGUISupportQt
    vtkRenderingCore
    vtkRenderingFreeTypeOpenGL
    vtkCommonCore
    vtkFiltersCore
    vtkInfovisCore
    vtkInteractionStyle
    vtkViewsQt
    NO_MODULE
    REQUIRED
  )
else()
  find_package(ItkVtkGlue REQUIRED)
  include(${ItkVtkGlue_USE_FILE})
    set(Glue ItkVtkGlue)
endif()

set(CMAKE_AUTOMOC ON)
find_package(Qt5Widgets REQUIRED)

add_subdirectory(source)
