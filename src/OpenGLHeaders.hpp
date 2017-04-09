#pragma once

// Some autocompletion tools doesnt really like OGL since it uses
// function pointers. It is therefore better to just use the raw OpenGL headers.
// We dont want this to happen on build though, so a define is made to make
// this possible.
#ifdef USE_OPENGL_RAW_HEADERS_AUTCOMPLETE
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#else
#include <OGL/OGL-3-3.hpp>
#endif

// OGL must always be first. Add line break so that clang-format doesnt
// reorder the order of them.
#include <GLFW/glfw3.h>
