include_directories(${visualize_SOURCE_DIR}/include)
add_executable(visualize MACOSX_BUNDLE Visualize.cpp)
target_link_libraries(visualize ${Glue} ${VTK_LIBRARIES} ${ITK_LIBRARIES})

