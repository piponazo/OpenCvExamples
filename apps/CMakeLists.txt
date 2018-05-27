link_libraries(opencv_highgui)

add_executable(resize_window_qt                 resize_window_qt.cpp)

add_executable(trackbar_control_panel           trackbar_control_panel.cpp)

add_executable(delaunay_gps delaunay_gps.cpp    linearscale.h linearscale.cpp)

add_executable(keypoint_selection_sdc           keypoint_selection_sdc.cpp)
target_link_libraries(keypoint_selection_sdc PRIVATE opencv_features2d opencv_imgproc)

add_executable(compare_circle_function          compare_circle_function.cpp)
target_link_libraries(compare_circle_function PRIVATE opencv_imgproc)

add_executable(imagelist_creator                imagelist_creator.cpp)
