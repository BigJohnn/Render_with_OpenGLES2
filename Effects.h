#ifndef _SHADER_H
#define _SHADER_H

#include "GLIncludes.h"
#include "esUtil.h"

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

#include <gl\SOIL.h>
#include "Mesh.h"
#include "Camera.h"
#include <opencv2\opencv.hpp>

class Effects
{
public:
	vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	GLuint facetex;

public:
	GLSLProgramPtr Program;
	GLSLProgramPtr depthProgram;
	GLSLProgramPtr quadProgram;
	GLSLProgramPtr particleProgram;

	// Constructor generates the shader on the fly
	Effects(const GLchar* vertexPath, const GLchar* fragmentPath);
	void windowInformation(GLuint w, GLuint h);
	/*  Functions   */
	// Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes std::vector.
	void LoadMeshes(std::string path);
	void Draw();
private:
	/*  Render data  */
	GLuint VBO, EBO;
	GLuint PVBO;//particles vbo
	GLuint quadVBO;
	// Data to fill
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
	GLuint texture[4];//默认支持4张外置纹理
	GLuint cubeTexture;

	glm::vec3 position;
	glm::vec3 up;
	glm::vec3 center;

protected:
	GLuint screenWidth, screenHeight;

	void processNode(aiNode* node, const aiScene* scene);

	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	// Checks all material textures of a given type and loads the textures if they're not loaded yet.
	// The required info is returned as a Texture struct.
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	
public:
	void loaduniforms4light();
	void loadUniforms(effects eff);
	void DEPTHsetUp();
	void QUADsetUp();
	void DrawQuad();
public:
	void loadTextures(effects eff);
private:
	GLint TextureFromFile(const char* path, std::string directory);
	GLuint loadSingleTexture(string img_path, int pos);
	void setupBufferObjects();
	void loaduniforms_fire();
	void loaduniforms_polkadot();
	void loaduniforms_phong();
	void loaduniforms_toon();
	void loaduniforms_glass();
	void loaduniforms_brick();
	void loaduniforms_normal();
	void loaduniforms_normalMap();
	void loaduniforms_worble();
	void loaduniforms_tile();
	void loaduniforms_rainbow();
	void loaduniforms_pool();
	GLuint loadCubeTextures(string path);
	void loaduniforms_envmap();
	void loaduniforms_cloud();
public:
	void load_particle_texture();
	void PARTICLEsetUp();
	GLuint loadSingleTextureFromMat(cv::Mat image);
private:
	void loaduniforms_makeup_test();
};


#endif