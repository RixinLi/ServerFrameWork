cmake_minimum_required(VERSION 2.8)
project(sylar)


set(CMAKE_VERBOSE_MAKEFILE ON)
#-Werror
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -fpermissive -Wno-unused-function")

include_directories(.)
include_directories(/apps/yaml-cpp/include)
link_directories(/apps/yaml-cpp/lib)

find_library(YAMLCPP yaml-cpp)


set(LIB_SRC
    sylar/address.cc
    sylar/config.cc
    sylar/fd_manager.cc
    sylar/fiber.cc
    sylar/hook.cc
    sylar/iomanager.cc
    sylar/log.cc
    sylar/scheduler.cc
    sylar/timer.cc
    sylar/thread.cc
    sylar/util.cc
    )

add_library(sylar SHARED ${LIB_SRC})
#add_library(sylar_static STATIC ${LIB_SRC})
#SET_TARGET_PROPERTIES (sylar_static PROPERTIES OUTPUT_NAME "sylar")

set(LIBS
        sylar
        dl
        pthread
        yaml-cpp
    )



add_executable(test tests/test.cc)
add_dependencies(test sylar)
target_link_libraries(test "${LIBS}")

add_executable(test_config tests/test_config.cc)
add_dependencies(test_config sylar)
target_link_libraries(test_config "${LIBS}")

add_executable(test_thread tests/test_thread.cc)
add_dependencies(test_thread sylar)
target_link_libraries(test_thread "${LIBS}")

add_executable(test_util tests/test_util.cc)
add_dependencies(test_util sylar)
target_link_libraries(test_util "${LIBS}")

add_executable(test_fiber tests/test_fiber.cc)
add_dependencies(test_fiber sylar)
target_link_libraries(test_fiber "${LIBS}")

add_executable(test_scheduler tests/test_scheduler.cc)
add_dependencies(test_scheduler sylar)
target_link_libraries(test_scheduler "${LIBS}")


add_executable(test_iomanager tests/test_iomanager.cc)
add_dependencies(test_iomanager sylar)
target_link_libraries(test_iomanager "${LIBS}")

add_executable(test_hook tests/test_hook.cc)
add_dependencies(test_hook sylar)
target_link_libraries(test_hook "${LIBS}")

add_executable(test_address tests/test_address.cc)
add_dependencies(test_address sylar)
target_link_libraries(test_address "${LIBS}")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)