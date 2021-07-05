cmake_minimum_required(VERSION 2.6)

PROJECT(Document_Scanner)


FIND_PACKAGE(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})
add_definitions(${OpenCV_DEFINITIONS})

SET(SRC documentScanner.cpp)

ADD_EXECUTABLE(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})