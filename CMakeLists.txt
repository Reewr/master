cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

project(DDDGP CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

include(cotire)

# Find & add TinyXML2
# This must be prior to glfw as they both add the custom target `uninstall`.
# glfw, unlike TinyXML, actually checks whether `uninstall` already exists.
# Added issue: https://github.com/leethomason/tinyxml2/issues/516
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/tinyxml2)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/tinyxml2)

# Find & add GLFW
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw/include)

# Find & add OGL (OpenGL Loading Library)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/OGL/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/OGL/include)

# Find & add SOIL (Simple OpenGL Image Library)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/SOIL/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/SOIL/src)

# Find & add backward-cpp
if (NOT WIN32)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/backward-cpp)
endif(NOT WIN32)

# Find & add FreeType2
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/freetype-2.5.3)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/freetype-2.5.3/include)

# find & add assimp, also set flags
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/assimp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/assimp/include)

# Set this later so ASSIMP doesnt add the binary files
# to root
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

#Find & add MMM
#include_directories(src)
#add_subdirectory(src/Math)

include_directories(src)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic-errors -g -std=c++11")

FILE(GLOB_RECURSE DDDGP_GLOB_CPP ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
FILE(GLOB_RECURSE DDDGP_GLOB_HPP ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

# Add backward to the exectuable, but only do so
# if the platform isnt windows
if (NOT WIN32)
  add_executable(DDDGP ${DDDGP_GLOB_CPP} ${DDDGP_GLOB_HPP} ${BACKWARD_ENABLE})
  add_backward(DDDGP)
else (NOT WIN32)
  add_executable(DDDGP ${DDDGP_GLOB_CPP} ${DDDGP_GLOB_HPP})
endif (NOT WIN32)

# Find the libraries that we've just added since the CMake documentation
# advises against using `set_link_directory`
find_library(TINYXML2_LIBRARY NAME tinyxml2)
find_library(GLFW_LIBRARY NAME glfw)
find_library(OGL_LIBRARY NAME ogl)
find_library(SOIL_LIBRARY NAME SOIL)
find_library(FREETYPE_LIBRARY NAME freetype)
find_library(ASSIMP_LIBRARY NAME assimp)

#
# Linking & stuff
#

# Note regarding TinyXML2:
# ------------------------
# Due to a strange usecase in TinyXML2, there's some issue with undefined
# references. Two of the functions are defined inline, `GetErrorStr1` and
# `GetErrorStr2` that uses `GetStr()` which is not defined inline. This
# causes the linker to throw undefined reference to `GetStr()`. Adding
# the cpp-file to the link library fixes this error.
#
# An issue (https://github.com/leethomason/tinyxml2/issues/498) has already
# been made regarding this, but the maintainer / author said:
# "No way to address / test. Would accept a PR"
# ------------------------
target_link_libraries(DDDGP tinyxml2
                      ${CMAKE_CURRENT_SOURCE_DIR}/deps/tinyxml2/tinyxml2.cpp)
target_link_libraries(DDDGP glfw ${GLFW_LIBRARIES})
target_link_libraries(DDDGP ogl)
target_link_libraries(DDDGP SOIL)
target_link_libraries(DDDGP freetype)
target_link_libraries(DDDGP assimp)

cotire(DDDGP)

add_custom_target(run COMMAND DDDGP DEPENDS DDDGP WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

if(WIN32)
  add_custom_target(
    cleanbuild
    COMMAND
    cd .. & rmdir /S /Q build & mkdir build & cd build & cmake -G \"MinGW Makefiles\" .. & mingw31-make -j5
    WORKING_DIRECTORY
    "${CMAKE_SOURCE_DIR}/build"
  )
endif(WIN32)
