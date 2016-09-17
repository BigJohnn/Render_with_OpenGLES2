#ifndef _DEBUG_H
#define _DEBUG_H

#include "GLIncludes.h"
//#define shadow_debug
//#define env_debug
//#define quad_debug

GLenum glCheckError_(const char *file, int line);
GLenum CheckFramebufferStatus(GLenum target);
#define glCheckError() glCheckError_(__FILE__, __LINE__)

#endif