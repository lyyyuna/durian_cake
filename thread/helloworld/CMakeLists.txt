cmake_minimum_required (VERSION 2.8)
set(APPNAME hello)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
project(${APPNAME})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)


aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src DIR_SRCS)
add_executable(${APPNAME} ${DIR_SRCS})

target_link_libraries(${APPNAME})
