#include "stdafx.h"
#include "Debug.h"

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}

GLenum CheckFramebufferStatus(GLenum target)
{
	GLenum errorCode;
	if ((errorCode = glCheckFramebufferStatus(target)) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:                  error = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:                 error = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:             error = "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS"; break;
		case GL_FRAMEBUFFER_UNSUPPORTED:                error = "GL_FRAMEBUFFER_UNSUPPORTED"; break;
		}
		std::cout << "[WARNING]FBO incompletement, \n[error code]" << error << std::endl;
	}
	return errorCode;
}