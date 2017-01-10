cmake_minimum_required(VERSION 2.8)

# =======================================
# Project initialization
# =====================================

# Set the project name and what type of program it is
project(DDDGP CXX)

# Export compile commands to a JSON file
# so that it can be used by IDEs if needed
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Set the primary compiler.
#
# TODO: We can remove this in favour of whatever
# the environment has as compiler when we know that both g++ and clang++
# builds correctly.
set(CMAKE_CXX_COMPILER g++)

# Tell CMake where to look for CMake modules and where to place lib/bin files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

# Cotire is a tool used to optimize the builds. We have to include it like this
include(cotire)


# =======================================
# Dependency inclusion
# =======================================

# Find & add TinyXML2
#
# Note:
# -----
# This must be prior to glfw as they both add the custom target `uninstall`.
# glfw, unlike TinyXML, actually checks whether `uninstall` already exists.
#
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

# Find & add backward-cpp if not on windows
if (NOT WIN32)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/backward-cpp)
endif(NOT WIN32)

# Find & add FreeType2
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/freetype-2.5.3)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/freetype-2.5.3/include)

# Find & add assimp, also set flags
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/assimp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/assimp/include)

# Set this later so ASSIMP doesnt add the binary files
# to root
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})


# =======================================
# Project files
# =======================================

# Set the include directories so that we can actually
# find our headers when we specify them.
include_directories(src)

# Set the compiler and flags
# TODO:
# Set to preferred compiler after we get it working with both g++ and clang++
# Also, set flags correctly. This is not correctly.
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic-errors -g -std=c++11")

# Add files
# TODO: Remove globbing and include specifically.
FILE(GLOB_RECURSE DDDGP_GLOB_CPP ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
FILE(GLOB_RECURSE DDDGP_GLOB_HPP ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)


# =======================================
# Dependency inclusion and linking
# =======================================

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

# =======================================
# Custom commands
# =======================================

# Perform cotire on our executable to optimize the build
cotire(DDDGP)

# Add a custom `run` command so that you can do `ninja run` to run the
# the executable.
add_custom_target(run COMMAND DDDGP DEPENDS DDDGP WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# Add special clean command if on windows
if(WIN32)
  add_custom_target(
    cleanbuild
    COMMAND
    cd .. & rmdir /S /Q build & mkdir build & cd build & cmake -G \"MinGW Makefiles\" .. & mingw31-make -j5
    WORKING_DIRECTORY
    "${CMAKE_SOURCE_DIR}/build"
  )
endif(WIN32)
