cmake_minimum_required(VERSION 3.0)
project(overlay-reader)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(FindPkgConfig)
include(GNUInstallDirs)

pkg_check_modules(ZIP REQUIRED libzip)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

include_directories(
    src
)

add_subdirectory(src)
#add_subdirectory(tools)
