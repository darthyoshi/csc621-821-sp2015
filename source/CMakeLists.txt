include_directories(
  ${visualize_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(QT_QMAKE_EXECUTABLE ${VTK_QT_QMAKE_EXECUTABLE} CACHE FILEPATH "")
set(QT_MOC_EXECUTABLE ${VTK_QT_MOC_EXECUTABLE} CACHE FILEPATH "")
set(QT_UIC_EXECUTABLE ${VTK_QT_UIC_EXECUTABLE} CACHE FILEPATH "")

file(GLOB UI_FILES *.ui)
file(GLOB QT_WRAP ${visualize_SOURCE_DIR}/include/*.h)
file(GLOB CPP_FILES *.cpp)

qt5_wrap_ui(UI_SRCS ${UI_FILES})
add_executable(windgrp MACOSX_BUNDLE ${CPP_FILES} ${UI_SRCS} ${QT_WRAP})
qt5_use_modules(windgrp Core Gui Widgets)
target_link_libraries(windgrp ${VTK_LIBRARIES} ${ITK_LIBRARIES})

