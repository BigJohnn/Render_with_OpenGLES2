// The MIT License (MIT)
//
// Copyright (c) 2016 Hou Peihong
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// This cpp is the logical entrypoint of the app.

#include "stdafx.h"
#include "esUtil.h"
#include "UserInterface.h"
#include "Debug.h"

using namespace std;

GLuint PVBO;
// Initialize the shader and program object
//
int Init(ESContext *esContext)
{
	UserData *userData = (UserData *)esContext->userData;

	//glEnable(GL_ALPHA_TEST_QCOM);
	glEnable(GL_CULL_FACE);//必须打开，否则显示结果不正确
	glCullFace(GL_BACK);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearDepthf(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	userData->width = esContext->width;
	userData->height = esContext->height;

	UI(userData);//Query and Input & Choose a shader to render.
	//InitFont(userData);//To display some shader information.

	return TRUE;
}

void Update(ESContext *esContext, float deltaTime)
{
	UserData *userData = (UserData *)esContext->userData;

	userData->particle_time += deltaTime;

	userData->particles->particleProgram->use();
	//cout << userData->particle_time << endl;
	if (userData->particle_time >= 1.0f)
	{
		float centerPos[3];
		float color[4];
		

		userData->particle_time = 0.0f;

		// Pick a new start location and color
		centerPos[0] = ((float)(rand() % 10000) / 10000.0f) - 0.5f;
		centerPos[1] = ((float)(rand() % 10000) / 10000.0f) - 0.5f;
		centerPos[2] = ((float)(rand() % 10000) / 10000.0f) - 0.5f;

		glUniform3fv(userData->particles->particleProgram->uniform("centerPosition"), 1, &centerPos[0]);

		// Random color
		color[0] = ((float)(rand() % 10000) / 20000.0f) + 0.5f;
		color[1] = ((float)(rand() % 10000) / 20000.0f) + 0.5f;
		color[2] = ((float)(rand() % 10000) / 20000.0f) + 0.5f;
		color[3] = 0.5;

		glUniform4f(userData->particles->particleProgram->uniform("ucolor"), color[0],color[1],color[2],color[3]);

	}
	// Load uniform time variable
	glUniform1f(userData->particles->particleProgram->uniform("Time"), userData->particle_time);
	userData->particles->particleProgram->release();
}

void Draw_Particles(UserData* userData)
{
	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glBindBuffer()
	// Use the program object
	userData->particles->particleProgram->use();
	glBindBuffer(GL_ARRAY_BUFFER, PVBO);
	glEnableVertexAttribArray(ATTRIBUTE_LIFETIME_LOCATION);
	// Load the vertex attributes
	glVertexAttribPointer(ATTRIBUTE_LIFETIME_LOCATION, 1, GL_FLOAT,
		GL_FALSE, PARTICLE_SIZE * sizeof(GLfloat),
		userData->particleData);

	glEnableVertexAttribArray(ATTRIBUTE_ENDPOSITION_LOCATION);
	glVertexAttribPointer(ATTRIBUTE_ENDPOSITION_LOCATION, 3, GL_FLOAT,
		GL_FALSE, PARTICLE_SIZE * sizeof(GLfloat),
		&userData->particleData[1]);

	glEnableVertexAttribArray(ATTRIBUTE_STARTPOSITION_LOCATION);
	glVertexAttribPointer(ATTRIBUTE_STARTPOSITION_LOCATION, 3, GL_FLOAT,
		GL_FALSE, PARTICLE_SIZE * sizeof(GLfloat),
		&userData->particleData[4]);

	// Blend particles
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind the texture
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, userData->particle_tex);

	// Set the sampler texture unit to 0
	//glUniform1i(userData->particles->particleProgram->uniform("stexture"), 10);
	glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);

	userData->particles->particleProgram->release();
}
// Draw the world.
//
void Draw(ESContext *esContext)
{
	UserData *userData = (UserData *)esContext->userData;

	//glClearColor(0.8902, 0.9294, 0.8039, 1.0);//樱
	glClearColor(0.0314, 0.1451, 0.4039, 1.0);//宝石蓝
	//glClearDepthf(0.5);
	// Set the viewport
	glViewport(0, 0, esContext->width, esContext->height);

	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if (userData->effects_enum == shadow)
	{
		glDisable(GL_BLEND);
		//绘制深度图
		glBindFramebuffer(GL_FRAMEBUFFER, userData->depthMapFBO);

		glViewport(0, 0, esContext->width, esContext->height);
		// Clear the color buffer
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);
		//glEnable(GL_DEPTH_TEST);
		userData->effect->depthProgram->use();
		userData->effect->loaduniforms4light();

		userData->effect->Draw();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glCullFace(GL_BACK);
		//glViewport(0, 0, esContext->width, esContext->height);
		glViewport(0, 0, 1024, 1024);

		glClearColor(0.0314, 0.6451, 0.4039, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	}
	
#ifdef shadow_debug
	/*glViewport(0, 0, esContext->width, esContext->height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);*/
	userData->effect->quadProgram->use();

	int num = userData->effect->textures_loaded.size()+1;

	glActiveTexture(GL_TEXTURE0+num);
	glBindTexture(GL_TEXTURE_2D, userData->colorTex);
	glUniform1i(userData->effect->quadProgram->uniform("depthMap"), num);
		
	userData->effect->DrawQuad();

#elif defined quad_debug
	userData->effect->quadProgram->use();

	int num = userData->effect->textures_loaded.size() + 1;

	//glActiveTexture(GL_TEXTURE0 + num);
	glBindTexture(GL_TEXTURE_2D, userData->effect->facetex);
	glUniform1i(userData->effect->quadProgram->uniform("depthMap"), 0);

	//glDisable(GL_DEPTH_TEST);
	userData->effect->DrawQuad();
	//glEnable(GL_DEPTH_TEST);
#else
	
	userData->effect->Program->use();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	userData->effect->loadUniforms(userData->effects_enum);
	userData->effect->Draw();

	glFrontFace(GL_CCW);
	userData->effect->Draw();
	glFrontFace(GL_CW);
	
#endif
	
	//Draw_Particles(userData);
	//RenderText(*(userData->txt_shader), userData->effects_name + " effect.", 25.0f, 25.0f, 0.5f, glm::vec3(1.0, 0.3804f, 0.0f));
	/*GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	cout << err << endl;*/
	//RenderText(*(userData->txt_shader), "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
}

void Shutdown(ESContext *esContext)
{
	UserData *userData = (UserData *)esContext->userData;

	eglMakeCurrent(esContext->eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext(esContext->eglDisplay, esContext->eglContext);
	eglDestroySurface(esContext->eglDisplay, esContext->eglSurface);
	eglTerminate(esContext->eglDisplay);
	esContext->eglDisplay = EGL_NO_DISPLAY;

	glDeleteFramebuffers(1, &userData->depthMapFBO);
}

bool keys[1024];
void KeyBoard(ESContext *esContext, unsigned char key, int xpos, int ypos)
{
	if (key == '\x1b')//esc
		PostQuitMessage(0);

	Do_Movement(key);

	/*if (action == KEY_PRESS)
		keys[key] = true;
	else if (action == KEY_RELEASE)
		keys[key] = false;*/
}

void Mouse(ESContext *esContext, int xpos, int ypos)
{
	Process_Mouse(xpos, ypos);
}

void Wheel(ESContext *esContext, int z)
{
	Process_Wheel(z);
}

int esMain(ESContext *esContext)
{
	////注意这里的写法！！！ PS.  new 且仅new 一次！
	UserData *data = new UserData;
	esContext->userData = data;

	esCreateWindow(esContext, "VF-Rendering", 640, 480, ES_WINDOW_RGB);

	if (!Init(esContext))
	{
		return GL_FALSE;
	}

	esRegisterShutdownFunc(esContext, Shutdown);
	esRegisterDrawFunc(esContext, Draw);
	esRegisterKeyFunc(esContext, KeyBoard);
	esRegisterMouseFunc(esContext, Mouse);
	esRegisterWheelFunc(esContext, Wheel);
	//esRegisterUpdateFunc(esContext, Update);

	return GL_TRUE;
}
