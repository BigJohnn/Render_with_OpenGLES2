#pragma once

//glm
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <egl\egl.h>
#include <vector>

#include <string.h>
#include "SharedPointerDefines.h"
#include "GLSLShader.h"
#include "GLSLProgram.h"

using namespace std;

enum effects
{
	none = 0,
	fire,
	glass,
	normal,
	normalmap,
	phong,
	rainbow,
	toon,
	worble,
	polkadot,
	brick,
	pool,
	tile,
	envmap,
	cloud,
	shadow,
	Makeup
};

enum
{
	ATTRIB_POS = 0,
	ATTRIB_NRM,
	ATTRIB_TEX,
	ATTRIB_TAN,
	ATTRIB_BITAN,
	ATTRIB_FONT
};

#define ATTRIBUTE_LIFETIME_LOCATION       0
#define ATTRIBUTE_STARTPOSITION_LOCATION  1
#define ATTRIBUTE_ENDPOSITION_LOCATION    2

struct Vertex {
	// Position
	glm::vec3 Position;
	// Normal
	glm::vec3 Normal;
	// TexCoords
	glm::vec2 TexCoords;

	glm::vec3 Tangents;

	glm::vec3 BiTangents;
};

struct Texture {
	GLuint id;
	string type;
	aiString path;
};

class Mesh {
public:
	/*  Mesh Data  */
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;

	/*  Functions  */
	// Constructor
	Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		// Now that we have all the required data, set the vertex buffers and its attribute pointers.
		this->setupMesh();
	}

	// Render the mesh
	void Draw()
	{
		// Draw mesh
		glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	}
	
private:
	/*  Render data  */
	GLuint VBO, EBO;

	/*  Functions    */
	// Initializes all the buffer objects/arrays
	void setupMesh()
	{
		// Create buffers
		glGenBuffers(1, &this->VBO);
		glGenBuffers(1, &this->EBO);

		// Load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

		// Vertex Positions
		glEnableVertexAttribArray(ATTRIB_POS);
		glVertexAttribPointer(ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
		// Vertex Normals
		glEnableVertexAttribArray(ATTRIB_NRM);
		glVertexAttribPointer(ATTRIB_NRM, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
		// Vertex Texture Coords
		glEnableVertexAttribArray(ATTRIB_TEX);
		glVertexAttribPointer(ATTRIB_TEX, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));
		
		glEnableVertexAttribArray(ATTRIB_TAN);
		glVertexAttribPointer(ATTRIB_TAN, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Tangents));
		
		glEnableVertexAttribArray(ATTRIB_BITAN);
		glVertexAttribPointer(ATTRIB_BITAN, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, BiTangents));
	}
};
