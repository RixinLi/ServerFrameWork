cmake_minimum_required(VERSION 2.8)
project(sylar)


set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

include_directories(.)
include_directories(/apps/yaml-cpp/include)
link_directories(/apps/yaml-cpp/lib)

find_library(YAMLCPP yaml-cpp)


set(LIB_SRC
    sylar/log.cc
    sylar/util.cc
    sylar/config.cc
    sylar/thread.cc
    )

add_library(sylar SHARED ${LIB_SRC})
#add_library(sylar_static STATIC ${LIB_SRC})
#SET_TARGET_PROPERTIES (sylar_static PROPERTIES OUTPUT_NAME "sylar")

set(LIB_LIB
    sylar
    pthread)



add_executable(test tests/test.cc)
add_dependencies(test sylar)
target_link_libraries(test sylar yaml-cpp pthread)

add_executable(test_config tests/test_config.cc)
add_dependencies(test_config sylar)
target_link_libraries(test_config sylar yaml-cpp pthread)

add_executable(test_thread tests/test_thread.cc)
add_dependencies(test_thread sylar)
target_link_libraries(test_thread sylar yaml-cpp pthread)

add_executable(test_util tests/test_util.cc)
add_dependencies(test_util sylar)
target_link_libraries(test_util sylar yaml-cpp pthread)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)