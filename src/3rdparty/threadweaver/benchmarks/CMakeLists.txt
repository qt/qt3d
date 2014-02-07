find_package(Qt5Test 5.2.0 REQUIRED NO_MODULE)

if(NOT MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()

include_directories(
     ${CMAKE_CURRENT_BINARY_DIR}/../src/ThreadWeaver/
)

remove_definitions(-DQT_NO_CAST_FROM_ASCII)

########### next target ###############

set(QueueBenchmarks_SRCS QueueBenchmarks.cpp)

add_executable(ThreadWeaverQueueBenchmarks ${QueueBenchmarks_SRCS})
# add_test(ThreadWeaverQueueBenchmarks ThreadWeaverQueueBenchmarks)

target_link_libraries(ThreadWeaverQueueBenchmarks Qt5::Test KF5::ThreadWeaver)
