
cmake_minimum_required(VERSION 2.8)

project(test_modules)

find_package(Qt5Quick REQUIRED)

find_package(Qt53D REQUIRED)
find_package(Qt53DQuick REQUIRED)

include_directories(
  ${Qt53D_INCLUDE_DIRS}
  ${Qt53DQuick_INCLUDE_DIRS}
)

add_definitions(
  ${Qt53D_DEFINITIONS}
  ${Qt53DQuick_DEFINITIONS}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")

add_executable(mainapp main.cpp)
target_link_libraries(mainapp
  ${Qt53D_LIBRARIES}
  ${Qt53DQuick_LIBRARIES}
)
