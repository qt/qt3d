# Generated from assimp.pro.

cmake_minimum_required(VERSION 3.16)
project(config_test_assimp LANGUAGES C CXX)

foreach(p ${QT_CONFIG_COMPILE_TEST_PACKAGES})
    find_package(${p})
endforeach()

if(QT_CONFIG_COMPILE_TEST_LIBRARIES)
    link_libraries(${QT_CONFIG_COMPILE_TEST_LIBRARIES})
endif()
if(QT_CONFIG_COMPILE_TEST_LIBRARY_TARGETS)
    foreach(lib ${QT_CONFIG_COMPILE_TEST_LIBRARY_TARGETS})
        if(TARGET ${lib})
            link_libraries(${lib})
        endif()
    endforeach()
endif()

add_executable(${PROJECT_NAME}
    main.cpp
)
