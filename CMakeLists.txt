cmake_minimum_required(VERSION 3.11)

project(dbus-music VERSION 1.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)
set(SOURCES
  src/mpris_media_player.cpp
  src/main.cpp
)
add_executable(
  ${PROJECT_NAME}
  ${SOURCES}
)

# find the d-bus pakage using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(DBUS REQUIRED dbus-1)

# include d-bus headers from pkg-config
target_include_directories(${PROJECT_NAME} PRIVATE ${DBUS_INCLUDE_DIRS})
# link against the d-bus library
target_link_libraries(${PROJECT_NAME} ${DBUS_LIBRARIES})

# find the spdlog pakage (headless)
#find_package(spdlog REQUIRED)
#target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog_header_only)
