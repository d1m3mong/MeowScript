cmake_minimum_required(VERSION 3.20)

project(MeowScript LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
  ${CMAKE_SOURCE_DIR}/src/*.cpp
)

add_executable(meowscript ${SOURCES})

target_include_directories(meowscript PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/src
)

if(MSVC)
  target_compile_options(meowscript PRIVATE /W4)
else()
  target_compile_options(meowscript PRIVATE -Wall -Wextra -Wpedantic)
endif()

