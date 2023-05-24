cmake_minimum_required(VERSION 3.25)
project(GoogleEarthImageMerger)

set(CMAKE_CXX_STANDARD 23)

set(OpenCV_FOUND 1)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(GoogleEarthImageMerger main.cpp portable-file-dialogs.h)

set(OpenCV_LIBS opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs)

target_link_libraries(GoogleEarthImageMerger ${OpenCV_LIBS})
