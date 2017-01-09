cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_COMPILER g++)

project(HELLOTEST CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

include(cotire)

# Find & add GLFW
add_subdirectory(deps/glfw-3.0.4)
include_directories(deps/glfw-3.0.4/include)

# Find & add OGL (OpenGL Loading Library)
add_subdirectory(deps/OGL/)
include_directories(deps/OGL/include)

# Find & add SOIL (Simple OpenGL Image Library)
add_subdirectory(deps/SOIL/)
include_directories(deps/SOIL/src)

# Find & add TinyXML2
add_subdirectory(deps/tinyxml2)
include_directories(deps/tinyxml2)

# Find & add backward-cpp
if(NOT WIN32)
  add_subdirectory(deps/backward-cpp)
  include_directories(deps/backward-cpp)
endif(NOT WIN32)

# Find & add FreeType2
add_subdirectory(deps/freetype-2.5.3)
include_directories(deps/freetype-2.5.3/include)

link_directories(${LIBRARY_OUTPUT_PATH})

#Find & add MMM
#include_directories(src)
#add_subdirectory(src/Math)

include_directories(src)

# Find & add ASSIMP
# Assimp is found in
# /usr/include
# C:/Mingw/include
# The reason for this is because we will
# hopefully, remove assimp as our loader

find_package(assimp REQUIRED)

if(ASSIMP_FOUND)
  include_directories(${ASSIMP_INCLUDE_DIR})
else (ASSIMP_FOUND)
  message("ERROR: Assimp not found")
endif (ASSIMP_FOUND)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic-errors -g -std=c++11")

# set(CMAKE_CXX_COMPILER g++)

FILE(GLOB_RECURSE DDDGP_GLOB_CPP src/*.cpp)
FILE(GLOB_RECURSE DDDGP_GLOB_HPP src/*.hpp)

add_executable(DDDGP ${DDDGP_GLOB_CPP} ${DDDGP_GLOB_HPP})

#
# Linking & stuff
#
target_link_libraries(DDDGP glfw ${GLFW_LIBRARIES})
target_link_libraries(DDDGP ogl)
target_link_libraries(DDDGP SOIL)
target_link_libraries(DDDGP tinyxml2)
target_link_libraries(DDDGP freetype)

if(NOT WIN32)
  target_link_libraries(DDDGP backward)
endif(NOT WIN32)

target_link_libraries(DDDGP ${ASSIMP_LIBRARY})

cotire(DDDGP)

add_custom_target(run COMMAND DDDGP DEPENDS DDDGP WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

if(WIN32)
  add_custom_target(
    cleanbuild
    COMMAND
    cd .. & rmdir /S /Q build & mkdir build & cd build & cmake -G \"MinGW Makefiles\" .. & mingw32-make -j5
    WORKING_DIRECTORY
    "${CMAKE_SOURCE_DIR}/build"
  )
endif(WIN32)
