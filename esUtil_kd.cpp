#include "stdafx.h"

#include "esUtil.h"

//////////////////////////////////////////////////////////////////
//
//  Public Functions
//
//

///
//  esCreateWindow()
//
//      title - name for title bar of window
//      width - width of window to create
//      height - height of window to create
//      flags  - bitwise or of window creation flags
//          ES_WINDOW_ALPHA       - specifies that the framebuffer should have alpha
//          ES_WINDOW_DEPTH       - specifies that a depth buffer should be created
//          ES_WINDOW_STENCIL     - specifies that a stencil buffer should be created
//          ES_WINDOW_MULTISAMPLE - specifies that a multi-sample buffer should be created
//
GLboolean ESUTIL_API esCreateWindow(ESContext *esContext, const char *title, GLint width, GLint height, GLuint flags)
{
	EGLConfig config;
	EGLint majorVersion;
	EGLint minorVersion;
	EGLint contextAttribs[] = { EGL_CONTEXT_MAJOR_VERSION_KHR, 2,
		EGL_CONTEXT_MINOR_VERSION_KHR, 0,
		EGL_NONE };

	if (esContext == NULL)
	{
		return GL_FALSE;
	}

	KDWindow *window = KD_NULL;
	// Use OpenKODE to create a Window
	window = kdCreateWindow(esContext->eglDisplay, config, KD_NULL);
	if (!window)
		kdExit(0);

	esContext->eglNativeDisplay = EGL_DEFAULT_DISPLAY;
	esContext->eglDisplay = eglGetDisplay(esContext->eglNativeDisplay);
	if (esContext->eglDisplay == EGL_NO_DISPLAY)
	{
		EGLint err = eglGetError();
		return GL_FALSE;
	}

	// Initialize EGL
	if (!eglInitialize(esContext->eglDisplay, &majorVersion, &minorVersion))
	{
		return GL_FALSE;
	}

   {
	   EGLint numConfigs = 0;
	   EGLint attribList[] =
	   {
		   EGL_LEVEL, 0,
		   EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		   EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,//opengles 2.0应该采取相应设置如左边
		   EGL_NATIVE_RENDERABLE, EGL_FALSE,
		   EGL_RED_SIZE, 5,
		   EGL_GREEN_SIZE, 6,
		   EGL_BLUE_SIZE, 5,
		   EGL_DEPTH_SIZE, EGL_DONT_CARE,
		   EGL_NONE
	   };

	   // Choose config
	   if (!eglChooseConfig(esContext->eglDisplay, attribList, &config, 1, &numConfigs))
	   {
		   return GL_FALSE;
	   }

	   if (numConfigs < 1)
	   {
		   return GL_FALSE;
	   }
   }

   // Create a surface
   esContext->eglSurface = eglCreateWindowSurface(esContext->eglDisplay, config,
	   esContext->eglNativeWindow, NULL);


   if (esContext->eglSurface == EGL_NO_SURFACE)
   {
	   EGLint err = eglGetError();
	   esContext->eglSurface = eglCreateWindowSurface(esContext->eglDisplay, config, NULL, NULL);
	   return GL_FALSE;
   }

   // Create a GL context
   esContext->eglContext = eglCreateContext(esContext->eglDisplay, config,
	   EGL_NO_CONTEXT, contextAttribs);

   if (esContext->eglContext == EGL_NO_CONTEXT)
   {
	   EGLint err = eglGetError();
	   return GL_FALSE;
   }

   // Make the context current
   if (!eglMakeCurrent(esContext->eglDisplay, esContext->eglSurface,
	   esContext->eglSurface, esContext->eglContext))
   {
	   return GL_FALSE;
   }

   return GL_TRUE;
}

///
//  esRegisterDrawFunc()
//
void ESUTIL_API esRegisterDrawFunc(ESContext *esContext, void (ESCALLBACK *drawFunc) (ESContext *))
{
	esContext->drawFunc = drawFunc;
}

///
//  esRegisterShutdownFunc()
//
void ESUTIL_API esRegisterShutdownFunc(ESContext *esContext, void (ESCALLBACK *shutdownFunc) (ESContext *))
{
	esContext->shutdownFunc = shutdownFunc;
}

///
//  esRegisterUpdateFunc()
//
void ESUTIL_API esRegisterUpdateFunc(ESContext *esContext, void (ESCALLBACK *updateFunc) (ESContext *, float))
{
	esContext->updateFunc = updateFunc;
}


///
//  esRegisterKeyFunc()
//
void ESUTIL_API esRegisterKeyFunc(ESContext *esContext,
	void (ESCALLBACK *keyFunc) (ESContext *, unsigned char, int, int))
{
	esContext->keyFunc = keyFunc;
}


///
// esLogMessage()
//
//    Log an error message to the debug output for the platform
//
void ESUTIL_API esLogMessage(const char *formatStr, ...)
{
	va_list params;
	char buf[BUFSIZ];

	va_start(params, formatStr);
	vsprintf(buf, formatStr, params);

#ifdef ANDROID
	__android_log_print(ANDROID_LOG_INFO, "esUtil", "%s", buf);
#else
	printf("%s", buf);
#endif

	va_end(params);
}
