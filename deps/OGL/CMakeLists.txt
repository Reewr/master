project(OGL CXX)
cmake_minimum_required(VERSION 2.8)
set(OGL_HEADER include/OGL/OGL-3-3.hpp)
set(OGL_SOURCES src/OGL-3-3.cpp)

add_library(ogl ${OGL_SOURCES} ${OGL_HEADER})

if(WIN32)
  if(MINGW)
    set_target_properties(ogl PROPERTIES IMPORT_SUFFIX "dll.a")
  else()
    set_target_properties(ogl PROPERTIES IMPORT_SUFFIX "dll.lib")
  endif()
  target_link_libraries(ogl LINK_INTERFACE_LIBRARIES)
endif()

install(TARGETS ogl EXPORT oglTargets DESTINATION lib)
install(DIRECTORY include/OGL DESTINATION include FILES_MATCHING PATTERN OGL3-3.hpp)
