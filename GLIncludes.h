#ifndef GLIncludes_h
#define GLIncludes_h

#if defined(__APPLE__) || defined(__MACOSX__)
#include <OpenGLES/ES2/glext.h>
#else

#include <EGL/egl.h>
#include <EGL/eglext.h>

#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#endif

#endif /* GLIncludes_h */
