link_libraries(${OpenCV_LIBS})

add_executable(resize_window_qt                 resize_window_qt.cpp)
add_executable(trackbar_control_panel           trackbar_control_panel.cpp)
add_executable(delaunay_gps delaunay_gps.cpp    linearscale.h linearscale.cpp)
add_executable(keypoint_selection_sdc           keypoint_selection_sdc.cpp)
add_executable(compare_circle_function          compare_circle_function.cpp)
