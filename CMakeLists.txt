cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

# =======================================
# Project initialization
# =====================================

# Set the project name and what type of program it is
project(Woooo CXX)

# Export compile commands to a JSON file
# so that it can be used by IDEs if needed
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Tell CMake where to place lib/bin files
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

if (NOT CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_BUILD_TYPE Debug)
endif()

# =======================================
# Dependency inclusion
# =======================================

# Macro to simply set the difference options of the dependencies
# Mostly used to not build tests and stuff
macro(SET_OPTION opt value)
  set(${opt} ${value} CACHE "" INTERNAL FORCE)
endmacro()

# build dependencies as shared libraries
set(BUILD_SHARED_LIBS ON)

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
SET_OPTION(GLFW_BUILD_EXAMPLES OFF)
SET_OPTION(GLFW_BUILD_TESTS OFF)
SET_OPTION(GLFW_BUILD_DOCS OFF)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw/include)

# Find & add OGL (OpenGL Loading Library)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/OGL/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/OGL/include)

# Find & add SOIL (Simple OpenGL Image Library)
SET_OPTION(SOIL_BUILD_TESTS OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/SOIL/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/SOIL/src)

# Find & add backward-cpp if not on windows
if (NOT WIN32)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/backward-cpp)
endif(NOT WIN32)

# Find & add FreeType2
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/freetype)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/freetype/include)

# Find & add assimp, also set flags
SET_OPTION(ASSIMP_BUILD_TESTS OFF)
SET_OPTION(ASSIMP_BUILD_ASSIMP_TOOLS OFF)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/assimp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/assimp/include)

# Find and add bulletphysics
SET_OPTION(USE_GRAPHICAL_BENCHMARK OFF)
SET_OPTION(BUILD_CPU_DEMOS OFF)
SET_OPTION(BUILD_OPENGL3_DEMOS OFF)
SET_OPTION(BUILD_BULLET2_DEMOS OFF)
SET_OPTION(BUILD_EXTRAS OFF)
SET_OPTION(BUILD_UNIT_TESTS OFF)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/bullet)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/bullet/src)

# Find and add mmm
SET_OPTION(BUILD_MMM_TESTS OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/mmm)
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/mmm/src)

# Find and add LUA51
find_package(Lua 5.3 REQUIRED)

# Include sol2
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/sol2)

# Include MultiNEAT
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/MultiNEAT)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/MultiNEAT)

# =======================================
# Project files
# =======================================

# Set this later so ASSIMP doesnt add the binary files
# to root
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

message("=========================================")
message("-- Building ${CMAKE_BUILD_TYPE}")
message("=========================================")

# Set the include directories so that we can actually
# find our headers when we specify them.
include_directories(src)

# We must require C++11
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} \
  -Wall \
  -Wextra \
  -pedantic-errors \
  -O3 \
  -fdiagnostics-color=always
  -ftemplate-depth=512")

# Set lots of warnings. All warnings that are listed below -Wall
# are warnings that GCC does not automatically include due to their
# possible contraversy. Adding them now to see how bad it gets
# TODO: Go through and add needed warnings
set(WARN_FLAGS "-std=c++14 -Wall -Wextra -fdiagnostics-color=always")
set(WARN_FLAGS "${WARN_FLAGS} -Wshadow")
set(WARN_FLAGS "${WARN_FLAGS} -Wcast-qual -Wno-cast-align")
set(WARN_FLAGS "${WARN_FLAGS} -Wwrite-strings -Wsign-compare")
set(WARN_FLAGS "${WARN_FLAGS} -Wmissing-declarations")
set(WARN_FLAGS "${WARN_FLAGS} -Wredundant-decls -Wunreachable-code -Winline")
set(WARN_FLAGS "${WARN_FLAGS} -ferror-limit=1")
set(WARN_FLAGS "${WARN_FLAGS} -ftemplate-depth=512")
set(WARN_FLAGS "${WARN_FLAGS} -pedantic-errors")


# This will throw lots of warnings due to MMM and other libraries
# using undefined `defined`
# set(WARN_FLAGS "${WARN_FLAGS} -Wundef")

#
#set(WARN_FLAGS "${WARN_FLAGS} -float-equal")
#set(WARN_FLAGS "${WARN_FLAGS} -Wconversion")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${WARN_FLAGS}")

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Store the CPP files
set(SOURCE_FILES

  # Top level files
  ${SRC_DIR}/main.cpp
  ${SRC_DIR}/Engine.cpp

  # src/3D
  ${SRC_DIR}/3D/Camera.cpp
  ${SRC_DIR}/3D/Terrain.cpp
  ${SRC_DIR}/3D/Cube.cpp
  ${SRC_DIR}/3D/World.cpp

  # src/Console
  ${SRC_DIR}/Console/Console.cpp

  # src/Drawable
  ${SRC_DIR}/Drawable/Drawable.cpp
  ${SRC_DIR}/Drawable/Drawable3D.cpp

  # src/Input
  ${SRC_DIR}/Input/Input.cpp
  ${SRC_DIR}/Input/Event.cpp

  # src/Lua
  #  ${SRC_DIR}/Lua/Utils/LCFG.cpp
  #  ${SRC_DIR}/Lua/Console/LConsole.cpp
  #  ${SRC_DIR}/Lua/LEngine.cpp
  #  ${SRC_DIR}/Lua/Input/LEvent.cpp
  #  ${SRC_DIR}/Lua/State/LState.cpp
  #  ${SRC_DIR}/Lua/Shape/LRectangle.cpp
  #  ${SRC_DIR}/Lua/Math/LMath.cpp
  #  ${SRC_DIR}/Lua/Resource/LFont.cpp
  #  ${SRC_DIR}/Lua/GUI/LText.cpp
  #  ${SRC_DIR}/Lua/GUI/LMenu.cpp
  #  ${SRC_DIR}/Lua/GUI/LDropdown.cpp
  #  ${SRC_DIR}/Lua/GUI/LSlider.cpp
  #  ${SRC_DIR}/Lua/GUI/LInputbox.cpp
  #  ${SRC_DIR}/Lua/GUI/LWindow.cpp
  ${SRC_DIR}/Lua/LuaLib.cpp
  ${SRC_DIR}/Lua/Lua.cpp

  # src/GLSL
  ${SRC_DIR}/GLSL/Program.cpp
  ${SRC_DIR}/GLSL/Shader.cpp

  # src/GUI
  ${SRC_DIR}/GUI/Checkbox.cpp
  ${SRC_DIR}/GUI/Dropdown.cpp
  ${SRC_DIR}/GUI/GUI.cpp
  ${SRC_DIR}/GUI/Inputbox.cpp
  ${SRC_DIR}/GUI/Menu.cpp
  ${SRC_DIR}/GUI/Text.cpp
  ${SRC_DIR}/GUI/Slider.cpp
  ${SRC_DIR}/GUI/Tooltip.cpp
  ${SRC_DIR}/GUI/Window.cpp

  # src/GUIMenu
  ${SRC_DIR}/GUIMenu/OptionsMenu.cpp
  ${SRC_DIR}/GUIMenu/PauseMenu.cpp

  # src/Graphical
  ${SRC_DIR}/Graphical/Framebuffer.cpp

  # src/Shape
  ${SRC_DIR}/Shape/Rectangle.cpp
  ${SRC_DIR}/Shape/GL/Grid.cpp
  ${SRC_DIR}/Shape/GL/Grid3D.cpp
  ${SRC_DIR}/Shape/GL/Rectangle.cpp
  ${SRC_DIR}/Shape/GL/Shape.cpp
  ${SRC_DIR}/Shape/GL/Cube.cpp

  # src/Import
  ${SRC_DIR}/Import/Animation.cpp
  ${SRC_DIR}/Import/UILoader.cpp
  ${SRC_DIR}/Import/XML.cpp

  # src/Math

  # src/Resource
  ${SRC_DIR}/Resource/Font.cpp
  ${SRC_DIR}/Resource/Texture.cpp
  ${SRC_DIR}/Resource/Resource.cpp
  ${SRC_DIR}/Resource/ResourceManager.cpp

  # src/State
  ${SRC_DIR}/State/MainMenu.cpp
  ${SRC_DIR}/State/Master.cpp
  ${SRC_DIR}/State/State.cpp

  # src/Utils
  ${SRC_DIR}/Utils/Asset.cpp
  ${SRC_DIR}/Utils/CFG.cpp
  ${SRC_DIR}/Utils/Utils.cpp
  ${SRC_DIR}/Utils/str.cpp
)

set(HEADER_FILES

  # Top level headers
  ${SRC_DIR}/Engine.hpp
  ${SRC_DIR}/OpenGLHeaders.hpp

  # src/3D
  ${SRC_DIR}/3D/Camera.hpp
  ${SRC_DIR}/3D/Terrain.hpp
  ${SRC_DIR}/3D/Cube.hpp
  ${SRC_DIR}/3D/World.hpp

  # src/Console
  ${SRC_DIR}/Console/Console.hpp

  # src/Drawable
  ${SRC_DIR}/Drawable/Drawable.hpp
  ${SRC_DIR}/Drawable/Drawable3D.hpp

  # src/Input
  ${SRC_DIR}/Input/Input.hpp
  ${SRC_DIR}/Input/Event.hpp

  # src/Lua
  #  ${SRC_DIR}/Lua/Utils/LCFG.hpp
  #  ${SRC_DIR}/Lua/Console/LConsole.hpp
  #  ${SRC_DIR}/Lua/LEngine.hpp
  #  ${SRC_DIR}/Lua/Input/LEvent.hpp
  #  ${SRC_DIR}/Lua/State/LState.hpp
  #  ${SRC_DIR}/Lua/Shape/LRectangle.hpp
  #  ${SRC_DIR}/Lua/Math/LMath.hpp
  #  ${SRC_DIR}/Lua/Resource/LFont.hpp
  #  ${SRC_DIR}/Lua/GUI/LText.hpp
  #  ${SRC_DIR}/Lua/GUI/LMenu.hpp
  #  ${SRC_DIR}/Lua/GUI/LDropdown.hpp
  #  ${SRC_DIR}/Lua/GUI/LSlider.hpp
  #  ${SRC_DIR}/Lua/GUI/LInputbox.hpp
  #  ${SRC_DIR}/Lua/GUI/LWindow.hpp
  ${SRC_DIR}/Lua/LuaLib.hpp
  ${SRC_DIR}/Lua/Lua.hpp

  # src/GLSL
  ${SRC_DIR}/GLSL/Program.hpp
  ${SRC_DIR}/GLSL/Shader.hpp

  # src/GUI
  ${SRC_DIR}/GUI/Checkbox.hpp
  ${SRC_DIR}/GUI/Dropdown.hpp
  ${SRC_DIR}/GUI/GUI.hpp
  ${SRC_DIR}/GUI/Inputbox.hpp
  ${SRC_DIR}/GUI/Menu.hpp
  ${SRC_DIR}/GUI/Slider.hpp
  ${SRC_DIR}/GUI/Text.hpp
  ${SRC_DIR}/GUI/Tooltip.hpp
  ${SRC_DIR}/GUI/Window.hpp

  # src/GUIMenu
  ${SRC_DIR}/GUIMenu/OptionsMenu.hpp
  ${SRC_DIR}/GUIMenu/PauseMenu.hpp

  # src/Graphical
  ${SRC_DIR}/Graphical/Framebuffer.hpp

  # src/Graphical/GL
  ${SRC_DIR}/Shape/Rectangle.hpp
  ${SRC_DIR}/Shape/GL/Grid.hpp
  ${SRC_DIR}/Shape/GL/Grid3D.hpp
  ${SRC_DIR}/Shape/GL/Rectangle.hpp
  ${SRC_DIR}/Shape/GL/Shape.hpp
  ${SRC_DIR}/Shape/GL/Cube.hpp

  # src/Import
  ${SRC_DIR}/Import/Animation.hpp
  ${SRC_DIR}/Import/UILoader.hpp
  ${SRC_DIR}/Import/XML.hpp

  # src/Resource
  ${SRC_DIR}/Resource/Font.hpp
  ${SRC_DIR}/Resource/Texture.hpp
  ${SRC_DIR}/Resource/Resource.hpp
  ${SRC_DIR}/Resource/ResourceManager.hpp

  # src/State
  ${SRC_DIR}/State/MainMenu.hpp
  ${SRC_DIR}/State/Master.hpp
  ${SRC_DIR}/State/State.hpp

  # src/Utils
  ${SRC_DIR}/Utils/Asset.hpp
  ${SRC_DIR}/Utils/CFG.hpp
  ${SRC_DIR}/Utils/Utils.hpp
  ${SRC_DIR}/Utils/str.hpp
)

# =======================================
# Dependency inclusion and linking
# =======================================

# Add backward-cpp to the exectuable, but only do so
# if the platform isnt windows
#if (NOT WIN32 AND CMAKE_BUILD_TYPE EQUAL "DEBUG")
#else (NOT WIN32)
#  add_executable(Woooo ${SOURCE_FILES} ${HEADER_FILES})
#endif ()
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/backward-cpp)
add_executable(Woooo ${SOURCE_FILES} ${HEADER_FILES} ${BACKWARD_ENABLE})
add_backward(Woooo)

# Find the libraries that we've just added since the CMake documentation
# advises against using `set_link_directory`
# TODO: Fix this as this block is incorrect use!
find_library(TINYXML2_LIBRARY NAME tinyxml2)
find_library(GLFW_LIBRARY NAME glfw)
find_library(OGL_LIBRARY NAME ogl)
find_library(SOIL_LIBRARY NAME SOIL)
find_library(FREETYPE_LIBRARY NAME freetype)
find_library(ASSIMP_LIBRARY NAME assimp)
find_library(BULLET_LIBRARY NAME bullet)
find_library(MultiNEAT_LIBRARY NAME MultiNEAT)

# Below follows defines that is defined for all our code base. These
# defines are defined here so that they're defined for all the code as
# defining them inside the main file did not work properly.
add_definitions(-DSOL_CHECK_ARGUMENTS=1)
add_definitions(-DSOL_SAFE_USERTYPE=1)
add_definitions(-DBACKWARD_HAS_DW=1)

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
target_link_libraries(Woooo ${LUA_LIBRARIES})
target_link_libraries(Woooo tinyxml2
                      ${CMAKE_CURRENT_SOURCE_DIR}/deps/tinyxml2/tinyxml2.cpp)
                    target_link_libraries(Woooo glfw ${GLFW_LIBRARIES})
                    target_link_libraries(Woooo ogl)
                    target_link_libraries(Woooo SOIL)
                    target_link_libraries(Woooo freetype)
                    target_link_libraries(Woooo assimp)
                    target_link_libraries(Woooo
                      BulletDynamics
                      BulletCollision)
                    target_link_libraries(Woooo mmm)
                    target_link_libraries(Woooo MultiNEAT)

# =======================================
# Custom commands
# =======================================

# Add a custom `run` command so that you can do `ninja run` to run the
# the executable.
add_custom_target(run COMMAND Woooo DEPENDS Woooo WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH} USES_TERMINAL)

# Add custom command for checking what build version we're using
add_custom_target(print_build_type COMMAND ${CMAKE_COMMAND} -E echo ${CMAKE_BUILD_TYPE})

# Add special clean commands
if(WIN32)
  add_custom_target(
    cleanbuild
    COMMAND
    cd .. & rmdir /S /Q build & mkdir build & cd build & cmake -G \"MinGW Makefiles\" .. & mingw31-make -j5
    WORKING_DIRECTORY
    "${CMAKE_SOURCE_DIR}/build")
else(WIN32)
  add_custom_target(
    cleanbuild
    COMMAND cd .. && rm -r build && mkdir build && rm Woooo && rm -r lib && rm -r bin
    WORKING_DIRECTORY
    "${CMAKE_SOURCE_DIR}/build")
endif(WIN32)
