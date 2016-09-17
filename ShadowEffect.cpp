#include "stdafx.h"
#include "Effects.h"

ShadowEffect::ShadowEffect(const GLchar* vertexPath, const GLchar* fragmentPath) :Effects(vertexPath, fragmentPath)
{
	this->depthProgram = createProgram(this->depthProgram, "simple.vert", "simple.frag");
	this->quadProgram = createProgram(this->quadProgram, "debug_quad_depth.vert", "debug_quad_depth.frag");
	//this->setup();

	screenWidth = 640;
	screenHeight = 480;
	position = glm::vec3(0.0f, 0.0f, 30.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	center = glm::vec3(0.f);
	//light
	glm::mat4 lightProjection, lightView;
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.f, 20.0f);
	//lightProjection = glm::perspective(45.0f, (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 100.f); // Note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene.
	//lightView = glm::lookAt(position, glm::vec3(0.0), up);
	lightView = glm::lookAt(position, glm::vec3(0.0), up);
	lightSpaceMatrix = lightProjection * lightView;
	model = glm::translate(model, glm::vec3(.0f, -18.2f, .0f));
	quadVBO = 0;
}

void ShadowEffect::setup()
{
	this->Program->bindAttribute("position", ATTRIB_POS);
	this->Program->bindAttribute("normal", ATTRIB_NRM);
	this->Program->bindAttribute("texCoords", ATTRIB_TEX);

	this->depthProgram->bindAttribute("position", ATTRIB_POS);

	this->quadProgram->bindAttribute("position", ATTRIB_POS);
	this->quadProgram->bindAttribute("normal", ATTRIB_NRM);
}

void ShadowEffect::loaduniforms4light()
{
	glUniformMatrix4fv(this->depthProgram->uniform("lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	glUniformMatrix4fv(this->depthProgram->uniform("model"), 1, GL_FALSE, glm::value_ptr(model));
}

void ShadowEffect::loaduniforms()
{
	// Transformation matrices
	glm::mat4 projection = glm::perspective(45.0f, (float)screenWidth / screenHeight, 0.1f, 100.0f);
	glm::mat4 view = glm::lookAt(position, center, up);

	glUniformMatrix4fv(this->Program->uniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(this->Program->uniform("view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 model_mat;
	model_mat = glm::translate(model_mat, glm::vec3(.0f, -18.2f, .0f)); // Translate it down a bit so it's at the center of the scene
	glUniformMatrix4fv(this->Program->uniform("model"), 1, GL_FALSE, glm::value_ptr(model_mat));
	glUniformMatrix3fv(this->Program->uniform("normMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(view*model_mat)))));
}

void ShadowEffect::DrawQuad()
{	
	if (quadVBO == 0)
	{
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		glGenBuffers(1, &quadVBO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void ShadowEffect::Draw(bool draw_depth)
{
	for (GLuint i = 0; i < this->meshes.size(); i++)
	{
		if (draw_depth)
			this->meshes[i].Draw();
		else
			this->meshes[i].shadowDraw(this->Program);
	}
		
}

GLSLProgramPtr ShadowEffect::createProgram(GLSLProgramPtr program, const GLchar* vertexPath, const GLchar* fragmentPath)
{
	// 1. Retrieve the vertex/fragment source code from filePath
	string vertexCode;
	string fragmentCode;
	try
	{
		// Open files
		ifstream vShaderFile(vertexPath);
		ifstream fShaderFile(fragmentPath);
		stringstream vShaderStream, fShaderStream;
		// Read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// Convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (exception e)
	{
		cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
	}
	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar * fShaderCode = fragmentCode.c_str();

	GLSLShaderPtr vertShader = std::make_shared<GLSLShader>();
	GLSLShaderPtr fragShader = std::make_shared<GLSLShader>();

	vertShader->create(GL_VERTEX_SHADER);
	vertShader->loadFromString(vShaderCode);
	vertShader->compile();

	fragShader->create(GL_FRAGMENT_SHADER);
	fragShader->loadFromString(fShaderCode);
	fragShader->compile();

	program = std::make_shared<GLSLProgram>();
	program->create();

	program->attach(vertShader);
	program->attach(fragShader);

	program->bindAttribute("position", ATTRIB_POS);
	program->bindAttribute("normal", ATTRIB_NRM);
	program->bindAttribute("texCoords", ATTRIB_TEX);
	program->bindAttribute("tangent", ATTRIB_TAN);
	program->bindAttribute("bitangent", ATTRIB_BITAN);
	
	program->link();

	program->detach(vertShader);
	program->detach(fragShader);
	return program;
}
