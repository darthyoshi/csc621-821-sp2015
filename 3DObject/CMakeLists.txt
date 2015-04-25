cmake_minimum_required(VERSION 2.6)
 
PROJECT(itkVtkImageConvert CXX)
 
FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})
#ImageToVTKImageFilter should be included in ITK build
 
FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})
 
ADD_EXECUTABLE(itkVtkImageConvert itkVtkImageConvert.cpp)
 
TARGET_LINK_LIBRARIES(itkVtkImageConvert ${ITK_LIBRARIES})
TARGET_LINK_LIBRARIES(itkVtkImageConvert ${VTK_LIBRARIES})