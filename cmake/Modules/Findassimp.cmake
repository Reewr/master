# Based on the FindPhysFS.cmake scipt
# - Try to find Assimp
# Once done this will define
#
#  ASSIMP_FOUND - system has Assimp
#  ASSIMP_INCLUDE_DIR - the Assimp include directory
#  ASSIMP_LIBRARY - Link these to use Assimp
set(ASSIMP_DEBUG ON)
set(_assimp_INCLUDE_SEARCH_DIRS_SYSTEM
    "C:/MinGW/include"
    "C:/MinGW/include/assimp"
    C:/assimp/include
    C:/assimp
    "$ENV{ProgramFiles}/assimp/include"
    "$ENV{ProgramFiles}/assimp"
    /sw/local/include
  )

set(_assimp_LIB_SEARCH_DIRS_SYSTEM
    "C:/MinGW/lib"
    "C:/MinGW"
    "C:/assimp/lib"
    "C:/assimp"
    "$ENV{ProgramFiles}/assimp/lib"
    "$ENV{ProgramFiles}/assimp"
    /sw/local/lib
  )

if(WIN32 AND NOT MINGW)
  set(CMAKE_FIND_LIBRARY_PREFIXES "")
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")
else(WIN32 AND NOT MINGW)
  set(CMAKE_FIND_LIBRARY_PREFIXES "lib")
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")
endif(WIN32 AND NOT MINGW)

FIND_PATH(ASSIMP_INCLUDE_DIR assimp/ai_assert.h
  $ENV{ASSIMPSDIR}/include
  $ENV{ASSIMPSDIR}
  $ENV{ASSIMPSDIR}/..
  ~/Library/Frameworks/AssImp.framework/Headers
  /Library/Frameworks/AssImp.framework/Headers
﻿  /usr/local/include/assimp
  /usr/local/include
  /usr/include/assimp
  /usr/include
  /sw/include/assimp # Fink
  /sw/include
﻿  /opt/local/include/assimp # DarwinPorts
  /opt/local/include
  /opt/csw/include/assimp # Blastwave
  /opt/csw/include
﻿  /opt/include/assimp
  /opt/include
  ${_assimp_INCLUDE_SEARCH_DIRS_SYSTEM}
)

FIND_LIBRARY(ASSIMP_LIBRARY_RELEASE
  NAMES assimp
  PATHS
  $ENV{ASSIMPSDIR}/lib
﻿  /usr/local/lib
  /usr/lib
  /sw/lib
  /opt/local/lib
  /opt/csw/lib
  /opt/lib
  ${_assimp_LIB_SEARCH_DIRS_SYSTEM}
)


set(ASSIMP_FOUND "NO")
if (ASSIMP_INCLUDE_DIR AND ASSIMP_LIBRARY_RELEASE)
  set(ASSIMP_FOUND "YES")
  set(ASSIMP_LIBRARY ${ASSIMP_LIBRARY_RELEASE})
endif(ASSIMP_INCLUDE_DIR AND ASSIMP_LIBRARY_RELEASE)

if(ASSIMP_DEBUG)
  message(STATUS "assimp inc: ${ASSIMP_INCLUDE_DIR}")
  message(STATUS "assimp lib: ${ASSIMP_LIBRARY}")
endif(ASSIMP_DEBUG)

if(AssImp_FIND_REQUIRED AND NOT (ASSIMP_LIBRARY AND ASSIMP_INCLUDE_DIR))
  message(FATAL_ERROR "Could not find assimp")
endif(AssImp_FIND_REQUIRED AND NOT (ASSIMP_LIBRARY AND ASSIMP_INCLUDE_DIR))

mark_as_advanced(ASSIMP_LIBRARY_RELEASE ASSIMP_INCLUDE_DIR)