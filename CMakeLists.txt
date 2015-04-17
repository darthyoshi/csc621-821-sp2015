cmake_minimum_required(VERSION 2.8)
 
project(visualize)
 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fpermissive -w -g")
set(EXECUTABLE_OUTPUT_PATH ${visualize_BINARY_DIR}/bin)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
if (ITKVtkGlue_LOADED)
  find_package(VTK 6.1 REQUIRED)
  include(${VTK_USE_FILE})
else()
  find_package(ItkVtkGlue REQUIRED)
  include(${ItkVtkGlue_USE_FILE})
    set(Glue ItkVtkGlue)
endif()

set(CMAKE_AUTOMOC ON)
find_package(Qt5Widgets REQUIRED)

add_subdirectory(source)
