project(tinyxml2)
cmake_minimum_required(VERSION 2.8)
set(TX_HEADER tinyxml2.h)
set(TX_SOURCE tinyxml2.cpp)

add_library(tinyxml2 ${TX_SOURCE} ${TX_HEADER})

if(WIN32)
  if(MINGW)
    set_target_properties(tinyxml2 PROPERTIES IMPORT_SUFFIX "dll.a")
  else()
    set_target_properties(tinyxml2 PROPERTIES IMPORT_SUFFIX "dll.lib")
  endif()
  target_link_libraries(tinyxml2 LINK_INTERFACE_LIBRARIES)
endif()

install(TARGETS tinyxml2 EXPORT txTargets DESTINATION lib)
install(DIRECTORY include/TinyXML2 DESTINATION include FILES_MATCHING PATTERN tinyxml2.h)