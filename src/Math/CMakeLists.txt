project(MMM CXX)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic-errors -g -std=c++11")

set(MMM_SOURCES 
  "mat3/mat3.cpp"
  "mat4/mat4.cpp"
  "vec2/vec2.cpp"
  "vec3/vec3.cpp"
  "vec4/vec4.cpp"
  "extra/print.cpp"
  "extra/camera.cpp"
  "MathCD.cpp"
)
add_library(MMM SHARED ${MMM_SOURCES} "Math.hpp")

if(WIN32)
  if(MINGW)
    set_target_properties(MMM PROPERTIES IMPORT_SUFFIX "dll.a")
  else()
    set_target_properties(MMM PROPERTIES IMPORT_SUFFIX "dll.lib")
  endif()
endif()

SET_TARGET_PROPERTIES(MMM PROPERTIES LINKER_LANGUAGE CXX)
install(TARGETS MMM EXPORT MMMTargets DESTINATION lib)
install(DIRECTORY include/MMM DESTINATION include FILES_MATCHING PATTERN Math.hpp)