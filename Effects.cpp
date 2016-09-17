#include "stdafx.h"
#include "Effects.h"

extern Camera camera;

using namespace std;
Effects::Effects(const GLchar* vertexPath, const GLchar* fragmentPath) :position(glm::vec3(0.0f, 0.0f, 30.0f)), up(glm::vec3(0.0f, 1.0f, 0.0f)), center(glm::vec3(0.f))
{
	GLSLShaderPtr vertShader = std::make_shared<GLSLShader>();
	GLSLShaderPtr fragShader = std::make_shared<GLSLShader>();

	vertShader->create(GL_VERTEX_SHADER);
	vertShader->loadFromFile(vertexPath);
	vertShader->compile();

	fragShader->create(GL_FRAGMENT_SHADER);
	fragShader->loadFromFile(fragmentPath);
	fragShader->compile();

	Program = std::make_shared<GLSLProgram>();
	Program->create();

	Program->attach(vertShader);
	Program->attach(fragShader);

	Program->bindAttribute("position", ATTRIB_POS);
	Program->bindAttribute("normal", ATTRIB_NRM);
	Program->bindAttribute("texCoords", ATTRIB_TEX);
	Program->bindAttribute("tangent", ATTRIB_TAN);
	Program->bindAttribute("bitangent", ATTRIB_BITAN);

	Program->link();

	Program->detach(vertShader);
	Program->detach(fragShader);
}

void Effects::LoadMeshes(string path)
{
	// Read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// Check for errors
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}

	// Retrieve the directory path of the filepath
	this->directory = path.substr(0, path.find_last_of('/'));

	// Process ASSIMP's root node recursively
	this->processNode(scene->mRootNode, scene);
}

void Effects::processNode(aiNode* node, const aiScene* scene)
{
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(this->processMesh(mesh, scene));
	}

	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene);
	}
}

Mesh Effects::processMesh(aiMesh* mesh, const aiScene* scene)
{
	// Walk through each of the mesh's vertices
	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector; // We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		// Positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// Normals
		if (mesh->mNormals != nullptr)
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}

		if (mesh->mTangents != nullptr)
		{
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangents = vector;
		}

		if (mesh->mBitangents != nullptr)
		{
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.BiTangents = vector;
		}
		// Texture Coordinates
		if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		vertices.push_back(vertex);
	}
	// Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// Retrieve all indices of the face and store them in the indices vector
		for (GLuint j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// Process materials
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		// 1. Diffuse maps
		vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. Specular maps
		vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. Reflection maps (Note that ASSIMP doesn't load reflection maps properly from wavefront objects, so we'll cheat a little by defining the reflection maps as ambient maps in the .obj file, which ASSIMP is able to load)
		vector<Texture> reflectionMaps = this->loadMaterialTextures(material, aiTextureType_AMBIENT, "reflection");
		textures.insert(textures.end(), reflectionMaps.begin(), reflectionMaps.end());
		// 4. Normal maps
		vector<Texture> normalMaps = this->loadMaterialTextures(material, aiTextureType_HEIGHT, "normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	}
	return Mesh(vertices, indices, textures);
}

vector<Texture> Effects::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		GLboolean skip = false;
		for (GLuint j = 0; j < textures_loaded.size(); j++)
		{
			if (textures_loaded[j].path == str)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // If texture hasn't been loaded already, load it
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			//texture.id = TextureFromFile(I);
			texture.type = typeName;
			texture.path = str;
			textures.push_back(texture);
			this->textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}

	return textures;
}

GLint Effects::TextureFromFile(const char* path, string directory)
{
	//Generate texture ID and load texture data 
	string filename = string(path);
	//filename = directory + '/' + filename;
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;
}

void Effects::setupBufferObjects()
{
	// Create buffers
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	// Load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);
}

void Effects::windowInformation(GLuint w, GLuint h)
{
	screenWidth = w;
	screenHeight = h;
}


void Effects::Draw()
{
	for (GLuint i = 0; i < this->meshes.size(); i++)
		this->meshes[i].Draw();
}

void Effects::loadUniforms(effects eff)
{
	switch (eff)
	{
	case none:
		break;
	case fire:loaduniforms_fire();
		break;
	case glass:loaduniforms_glass();
		break;
	case normal:loaduniforms_normal();
		break;
	case normalmap:loaduniforms_normalMap();
		break;
	case phong:loaduniforms_phong();
		break;
	case rainbow:loaduniforms_rainbow();
		break;
	case toon:loaduniforms_toon();
		break;
	case worble:loaduniforms_worble();
		break;
	case polkadot:loaduniforms_polkadot();
		break;
	case brick:loaduniforms_brick();
		break;
	case pool:loaduniforms_pool();
		break;
	case tile:loaduniforms_tile();
		break;
	case envmap:loaduniforms_envmap();
		break;
	case cloud:loaduniforms_cloud();
		break;
	case shadow:
		break;
	default:
		break;
	}
}

void Effects::DEPTHsetUp()
{
	GLSLShaderPtr vertShader = std::make_shared<GLSLShader>();
	GLSLShaderPtr fragShader = std::make_shared<GLSLShader>();

	vertShader->create(GL_VERTEX_SHADER);
	vertShader->loadFromFile("simple.vert");
	vertShader->compile();

	fragShader->create(GL_FRAGMENT_SHADER);
	fragShader->loadFromFile("simple.frag");
	fragShader->compile();

	depthProgram = std::make_shared<GLSLProgram>();
	depthProgram->create();

	depthProgram->attach(vertShader);
	depthProgram->attach(fragShader);

	depthProgram->bindAttribute("position", ATTRIB_POS);

	depthProgram->link();

	depthProgram->detach(vertShader);
	depthProgram->detach(fragShader);
}


void Effects::QUADsetUp()
{
	GLSLShaderPtr vertShader = std::make_shared<GLSLShader>();
	GLSLShaderPtr fragShader = std::make_shared<GLSLShader>();

	vertShader->create(GL_VERTEX_SHADER);
	vertShader->loadFromFile("debug_quad_depth.vert");
	vertShader->compile();

	fragShader->create(GL_FRAGMENT_SHADER);
	fragShader->loadFromFile("debug_quad_depth.frag");
	fragShader->compile();

	quadProgram = std::make_shared<GLSLProgram>();
	quadProgram->create();

	quadProgram->attach(vertShader);
	quadProgram->attach(fragShader);

	quadProgram->bindAttribute("position", 0);
	/*
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	!!!!!!!!!!!!!!!!!!!!!!!!!!与名称无关，与序号有关！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
	！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
	*/
	quadProgram->bindAttribute("texCoords", 1);

	quadProgram->link();

	quadProgram->detach(vertShader);
	quadProgram->detach(fragShader);

	quadVBO = 0;
}

void Effects::DrawQuad()
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
	//if (quadVBO == 0)
	//{
	//	GLfloat verti[] = {
	//		// Positions            // Texture Coords
	//		0.5f, 0.5f, 0.0f, 1.0f, 1.0f, // Top Right
	//		0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // Bottom Right
	//		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // Bottom Left
	//		-0.5f, 0.5f, 0.0f, 0.0f, 1.0f  // Top Left 
	//	};
	//	GLuint indi[] = {  // Note that we start from 0!
	//		0, 1, 3, // First Triangle
	//		1, 2, 3  // Second Triangle
	//	};
	//	GLuint _EBO;

	//	glGenBuffers(1, &quadVBO);
	//	glGenBuffers(1, &_EBO);

	//	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	//	glBufferData(GL_ARRAY_BUFFER, sizeof(verti), verti, GL_DYNAMIC_DRAW);

	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
	//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indi), indi, GL_DYNAMIC_DRAW);

	//	// Position attribute
	//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	//	glEnableVertexAttribArray(0);
	//	// TexCoord attribute
	//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//	glEnableVertexAttribArray(1);
	//}
	//
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Effects::loaduniforms4light()
{
	//light
	glm::mat4 lightProjection, lightView;
	lightProjection = glm::perspective(camera.Zoom, (float)screenWidth / screenHeight, 0.1f, 100.0f);// glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.f, 20.0f);
	lightView = glm::lookAt(position, glm::vec3(0.0), up);
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;
	glUniformMatrix4fv(this->depthProgram->uniform("lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(.0f, -18.2f, .0f));
	glUniformMatrix4fv(this->depthProgram->uniform("model"), 1, GL_FALSE, glm::value_ptr(model));
}

//GLuint &texture 注意要传入引用！！！
GLuint Effects::loadSingleTexture(string img_path, int pos)
{
	glGenTextures(1, &texture[pos]);

	int width, height, channel;
	unsigned char* image = SOIL_load_image(img_path.c_str(), &width, &height, &channel, SOIL_LOAD_AUTO);

	//unsigned char *img = new unsigned char[width*height*channel];
	//for (int i = 0; i < width; ++i) {
	//	for (int j = 0; j < height; ++j) {
	//		unsigned char *imgptr = img + (j*width + i)*channel;
	//		imgptr[0] = 255;
	//		imgptr[1] = 0;
	//		imgptr[2] = 0;
	//		imgptr[3] = 255;
	//	}
	//}

	// Assign texture to ID
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[pos]);

	if (channel == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	else if (channel == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	else
		cout << "ERROR Channel :" << channel << endl;

	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	//delete[]img;
	return texture[pos];
}

GLuint Effects::loadCubeTextures(string path)
{
	vector<string> faces;
	faces.push_back("rt.tga");
	faces.push_back("lf.tga");
	faces.push_back("up.tga");
	faces.push_back("dn.tga");
	faces.push_back("bk.tga");
	faces.push_back("ft.tga");
	// Generate a texture object
	glGenTextures(1, &cubeTexture);

	// Bind the texture object
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture);

	int width, height, channel;
	for (int i = 0; i < faces.size(); i++)
	{
		unsigned char* image = SOIL_load_image((path + "hangingstone_" + faces[i]).c_str(), &width, &height, &channel, SOIL_LOAD_AUTO);

		if (channel == 4)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else if (channel == 3)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		else
			cout << "ERROR Channel :" << channel << endl;
	}

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// Parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R_OES, GL_REPEAT);

	return cubeTexture;
}

void Effects::loaduniforms_fire()
{
	time_t t = time(0);
	t %= 60;
	float Time = 1.0*(float)t;
	glUniform1f(this->Program->uniform("Time"), Time);
	glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / screenHeight, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();//glm::lookAt(camera.Position, center, camera.Up);

	glUniformMatrix4fv(this->Program->uniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(this->Program->uniform("view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 model_mat;
	model_mat = glm::translate(model_mat, glm::vec3(.0f, -18.2f, .0f)); // Translate it down a bit so it's at the center of the scene
	model_mat = camera.GetRotateMatrix(model_mat);
	glUniformMatrix4fv(this->Program->uniform("model"), 1, GL_FALSE, glm::value_ptr(model_mat));
	glUniformMatrix3fv(this->Program->uniform("normMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(view*model_mat)))));
}

// 
void Effects::loaduniforms_polkadot()
{
	// Transformation matrices
	//= glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.f, 100.0f);
	glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / screenHeight, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();//glm::lookAt(camera.Position, center, camera.Up);

	//右手定则！！！！！
	glUniform4f(this->Program->uniform("lightPos"), .0f, 20.0f, 0.0f, 1.0f);//polkadot & normal mapping

	glUniform1f(this->Program->uniform("materialSpecularExponent"), 0.1f);
	//polkadot
	glUniform4f(this->Program->uniform("materialDiffuseColor0"), 0.996f, 0.875f, 0.882f, 1.0f);  //surface color
	glUniform4f(this->Program->uniform("materialDiffuseColor1"), .086f, .704f, .955f, 1.0);		//dot color

	glUniformMatrix4fv(this->Program->uniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(this->Program->uniform("view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 model_mat;
	model_mat = glm::translate(model_mat, glm::vec3(.0f, -18.2f, .0f)); // Translate it down a bit so it's at the center of the scene
	model_mat = camera.GetRotateMatrix(model_mat);
	glUniformMatrix4fv(this->Program->uniform("model"), 1, GL_FALSE, glm::value_ptr(model_mat));
	glUniformMatrix3fv(this->Program->uniform("normMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(view*model_mat)))));
}


void Effects::loaduniforms_phong()
{
	// Transformation matrices
	glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / screenHeight, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();//glm::lookAt(camera.Position, center, camera.Up);

	//右手定则！！！！！
	glUniform3f(this->Program->uniform("viewPos"), 0.f, 0.f, 30.f);

	// Set lights properties
	glUniform3f(this->Program->uniform("light.direction"), 0.0f, 0.0f, 1.0f);
	glUniform3f(this->Program->uniform("light.ambient"), 0.7f, 0.7f, 0.7f);
	glUniform3f(this->Program->uniform("light.diffuse"), 0.5f, 0.5f, 0.5f);
	glUniform3f(this->Program->uniform("light.specular"), 1.0f, 1.0f, 1.0f);
	// Set material properties
	glUniform1f(this->Program->uniform("material.shininess"), 32.0f);


	glUniformMatrix4fv(this->Program->uniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(this->Program->uniform("view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 model_mat;
	model_mat = glm::translate(model_mat, glm::vec3(.0f, -18.2f, .0f)); // Translate it down a bit so it's at the center of the scene
	model_mat = camera.GetRotateMatrix(model_mat);
	glUniformMatrix4fv(this->Program->uniform("model"), 1, GL_FALSE, glm::value_ptr(model_mat));
	glUniformMatrix3fv(this->Program->uniform("normMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(view*model_mat)))));

	for (GLuint i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
		string name = this->textures[i].type;
		string head = "material.";
		if (name == "diffuse")
			;
		else if (name == "specular")
			;
		else if (name == "reflection")	// We'll now also need to add the code to set and bind to reflection textures
			continue;
		else if (name == "normal")
			continue;
		glUniform1i(this->Program->uniform(head + name), i);
		// And finally bind the texture
		glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);
}


void Effects::loaduniforms_toon()
{
	// Transformation matrices
	glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / screenHeight, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();//glm::lookAt(camera.Position, center, camera.Up);

	//右手定则！！！！！
	glUniform3f(this->Program->uniform("viewPos"), 0.f, 0.f, 30.f);

	// Set lights properties
	glUniform4f(this->Program->uniform("light.pos"), 31.0f, 11.0f, 11.0f, 0.0f);//平行光
	glUniform3f(this->Program->uniform("light.ambient"), 0.7f, 0.7f, 0.7f);
	glUniform3f(this->Program->uniform("light.diffuse"), 0.5f, 0.5f, 0.5f);
	glUniform3f(this->Program->uniform("light.specular"), 1.0f, 1.0f, 1.0f);
	// Set material properties
	glUniform1f(this->Program->uniform("material.shininess"), 32.0f);


	glUniformMatrix4fv(this->Program->uniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(this->Program->uniform("view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 model_mat;
	model_mat = glm::translate(model_mat, glm::vec3(.0f, -18.2f, .0f)); // Translate it down a bit so it's at the center of the scene
	model_mat = camera.GetRotateMatrix(model_mat);
	glUniformMatrix4fv(this->Program->uniform("model"), 1, GL_FALSE, glm::value_ptr(model_mat));
	glUniformMatrix3fv(this->Program->uniform("normMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(view*model_mat)))));

	for (GLuint i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
		string name = this->textures[i].type;
		string head = "material.";
		if (name == "diffuse")
			;
		else if (name == "specular")
			;
		else if (name == "reflection")	// We'll now also need to add the code to set and bind to reflection textures
			continue;
		else if (name == "normal")
			continue;
		glUniform1i(this->Program->uniform(head + name), i);
		// And finally bind the texture
		glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);
}


void Effects::loaduniforms_glass()
{
	// Transformation matrices
	glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / screenHeight, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();//glm::lookAt(camera.Position, center, camera.Up);

	//右手定则！！！！！
	glUniform3f(this->Program->uniform("viewPos"), 0.f, 0.f, 30.f);
	glUniform4f(this->Program->uniform("lightPos"), 31.0f, 11.0f, 11.0f, 0.0f);//平行光

	glUniform4f(Program->uniform("GlassParams"), 0.8f, 0.7f, 0.9f, 0.9f);

	glUniformMatrix4fv(this->Program->uniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(this->Program->uniform("view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 model_mat;
	model_mat = glm::translate(model_mat, glm::vec3(.0f, -18.2f, .0f)); // Translate it down a bit so it's at the center of the scene
	model_mat = camera.GetRotateMatrix(model_mat);
	glUniformMatrix4fv(this->Program->uniform("model"), 1, GL_FALSE, glm::value_ptr(model_mat));
	glUniformMatrix3fv(this->Program->uniform("normMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(view*model_mat)))));

	int num = textures.size() + 1;
	glActiveTexture(GL_TEXTURE0 + num);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glUniform1i(Program->uniform("background"), num);
	glActiveTexture(GL_TEXTURE0);
}


void Effects::loaduniforms_brick()
{
	// Transformation matrices
	glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / screenHeight, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();//glm::lookAt(camera.Position, center, camera.Up);

	//右手定则！！！！！
	glUniform4f(this->Program->uniform("lightPos"), 31.0f, 31.0f, 31.0f, 0.0f);//平行光

	//brick
	glUniform4f(Program->uniform("materialDiffuseColor0"), .086f, .704f, .955f, 1.0);		//brick color
	glUniform4f(Program->uniform("materialDiffuseColor1"), .903f, .296f, .007f, 1.0);		//mortar color

	glUniformMatrix4fv(this->Program->uniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(this->Program->uniform("view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 model_mat;
	model_mat = glm::translate(model_mat, glm::vec3(.0f, -18.2f, .0f)); // Translate it down a bit so it's at the center of the scene
	model_mat = camera.GetRotateMatrix(model_mat);
	glUniformMatrix4fv(this->Program->uniform("model"), 1, GL_FALSE, glm::value_ptr(model_mat));
	glUniformMatrix3fv(this->Program->uniform("normMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(view*model_mat)))));
}


void Effects::loaduniforms_normal()
{
	// Transformation matrices
	glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / screenHeight, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();//glm::lookAt(camera.Position, center, camera.Up);

	//右手定则！！！！！
	glUniform3f(this->Program->uniform("viewPos"), 0.f, 0.f, 30.f);

	// Set lights properties
	glUniform4f(this->Program->uniform("lightPos"), 11.0f, 11.0f, 11.0f, 0.0f);//平行光
	glUniform3f(this->Program->uniform("light.ambient"), 0.7f, 0.7f, 0.7f);
	glUniform3f(this->Program->uniform("light.diffuse"), 0.5f, 0.5f, 0.5f);
	glUniform3f(this->Program->uniform("light.specular"), 1.0f, 1.0f, 1.0f);
	// Set material properties
	glUniform1f(this->Program->uniform("material.shininess"), 32.0f);


	glUniformMatrix4fv(this->Program->uniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(this->Program->uniform("view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 model_mat;
	model_mat = glm::translate(model_mat, glm::vec3(.0f, -18.2f, .0f)); // Translate it down a bit so it's at the center of the scene
	model_mat = camera.GetRotateMatrix(model_mat);
	glUniformMatrix4fv(this->Program->uniform("model"), 1, GL_FALSE, glm::value_ptr(model_mat));
	glUniformMatrix3fv(this->Program->uniform("normMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(view*model_mat)))));

	for (GLuint i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
		string name = this->textures[i].type;
		string head = "material.";
		if (name == "diffuse")
			;
		else if (name == "specular")
			;
		else if (name == "reflection")	// We'll now also need to add the code to set and bind to reflection textures
			;
		else if (name == "normal")
			;
		glUniform1i(this->Program->uniform(head + name), i);
		// And finally bind the texture
		glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);
}


void Effects::loaduniforms_normalMap()
{
	// Transformation matrices
	glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / screenHeight, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	//glm::lookAt(camera.Position, center, camera.Up);

	//右手定则！！！！！
	glUniform3f(this->Program->uniform("viewPos"), 0.f, 0.f, 30.f);

	// Set lights properties
	glUniform4f(this->Program->uniform("light.pos"), 11.0f, 11.0f, 11.0f, 0.0f);//平行光
	glUniform3f(this->Program->uniform("light.ambient"), 0.7f, 0.7f, 0.7f);
	glUniform3f(this->Program->uniform("light.diffuse"), 0.5f, 0.5f, 0.5f);
	glUniform3f(this->Program->uniform("light.specular"), 1.0f, 1.0f, 1.0f);
	// Set material properties
	glUniform1f(this->Program->uniform("material.shininess"), 32.0f);

	glUniformMatrix4fv(this->Program->uniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(this->Program->uniform("view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 model_mat;
	model_mat = glm::translate(model_mat, glm::vec3(.0f, -18.2f, .0f)); // Translate it down a bit so it's at the center of the scene
	model_mat = camera.GetRotateMatrix(model_mat);
	glUniformMatrix4fv(this->Program->uniform("model"), 1, GL_FALSE, glm::value_ptr(model_mat));
	glUniformMatrix3fv(this->Program->uniform("normMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(view*model_mat)))));

	for (GLuint i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
		string name = this->textures[i].type;
		string head = "material.";
		if (name == "diffuse")
			;
		else if (name == "specular")
			;
		else if (name == "reflection")	// We'll now also need to add the code to set and bind to reflection textures
			;
		else if (name == "normal")
			;
		glUniform1i(this->Program->uniform(head + name), i);
		// And finally bind the texture
		glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);
}


void Effects::loaduniforms_worble()
{
	time_t t = time(0);
	t %= 60;
	float Time = 1.0*(float)t;
	glUniform1f(this->Program->uniform("Time"), Time);
	// Transformation matrices
	glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / screenHeight, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();//glm::lookAt(camera.Position, center, camera.Up);

	//右手定则！！！！！
	glUniform3f(this->Program->uniform("viewPos"), 0.f, 0.f, 30.f);

	// Set lights properties
	glUniform4f(this->Program->uniform("light.pos"), 1.0f, 1.0f, 1.0f, 1.0f);//平行光
	glUniform3f(this->Program->uniform("light.ambient"), 0.7f, 0.7f, 0.7f);
	glUniform3f(this->Program->uniform("light.diffuse"), 0.5f, 0.5f, 0.5f);
	glUniform3f(this->Program->uniform("light.specular"), 1.0f, 1.0f, 1.0f);
	// Set material properties
	glUniform1f(this->Program->uniform("material.shininess"), 32.0f);


	glUniformMatrix4fv(this->Program->uniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(this->Program->uniform("view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 model_mat;
	model_mat = glm::translate(model_mat, glm::vec3(.0f, -18.2f, .0f)); // Translate it down a bit so it's at the center of the scene
	model_mat = camera.GetRotateMatrix(model_mat);
	glUniformMatrix4fv(this->Program->uniform("model"), 1, GL_FALSE, glm::value_ptr(model_mat));
	glUniformMatrix3fv(this->Program->uniform("normMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(view * model_mat)))));

	for (GLuint i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
		string name = this->textures[i].type;
		string head = "material.";
		if (name == "diffuse")
			;
		else if (name == "specular")
			;
		else if (name == "reflection")	// We'll now also need to add the code to set and bind to reflection textures
			continue;
		else if (name == "normal")
			continue;
		glUniform1i(this->Program->uniform(head + name), i);
		// And finally bind the texture
		glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);
}


void Effects::loaduniforms_tile()
{
	// Transformation matrices
	glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / screenHeight, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();//glm::lookAt(camera.Position, center, camera.Up);

	//右手定则！！！！！
	glUniform3f(this->Program->uniform("viewPos"), 0.f, 0.f, 30.f);

	// Set lights properties
	glUniform4f(this->Program->uniform("light.pos"), 1.0f, 1.0f, 0.0f, 0.0f);//平行光

	glUniform1f(Program->uniform("materialSpecularExponent"), 0.1f);
	glUniform4f(Program->uniform("materialSpecularColor"), 0.1f, 0.1f, 0.107f, 1.0);

	glUniformMatrix4fv(this->Program->uniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(this->Program->uniform("view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 model_mat;
	model_mat = glm::translate(model_mat, glm::vec3(.0f, -18.2f, .0f)); // Translate it down a bit so it's at the center of the scene
	model_mat = camera.GetRotateMatrix(model_mat);
	glUniformMatrix4fv(this->Program->uniform("model"), 1, GL_FALSE, glm::value_ptr(model_mat));
	glUniformMatrix3fv(this->Program->uniform("normMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(view * model_mat)))));

	int num = textures.size() + 1;
	glActiveTexture(GL_TEXTURE0 + num);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glUniform1i(Program->uniform("texture0"), num++);

	glActiveTexture(GL_TEXTURE0 + num);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glUniform1i(Program->uniform("texture1"), num++);

	glActiveTexture(GL_TEXTURE0);
}

void Effects::loadTextures(effects eff)
{
	if (eff == glass)
	{
		loadSingleTexture("bg.jpg", 0);
	}
	else if (eff == tile)
	{
		loadSingleTexture("soil.png", 0);
		loadSingleTexture("soil_norm.png", 1);
	}
	else if (eff == pool)
	{
		loadSingleTexture("cute.png", 0);
	}
	else if (eff == envmap)
	{
		loadCubeTextures("cube\\");
	}
	else if (eff == Makeup)
	{
		loadSingleTexture("image_uv.png", 0);
		loadSingleTexture("panda_hp.png", 1);
	}
}


void Effects::loaduniforms_rainbow()
{
	time_t t = time(0);
	t %= 60;
	float Time = 1.0*(float)t;
	glUniform1f(this->Program->uniform("Time"), Time);
	// Transformation matrices
	glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / screenHeight, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();//glm::lookAt(camera.Position, center, camera.Up);

	//右手定则！！！！！
	glUniform3f(this->Program->uniform("viewPos"), 0.f, 0.f, 30.f);

	// Set lights properties
	glUniform4f(this->Program->uniform("lightPos"), 1.0f, 1.0f, 0.0f, 0.0f);//平行光

	glUniform1f(Program->uniform("materialSpecularExponent"), 0.1f);
	glUniform4f(Program->uniform("materialSpecularColor"), 0.4f, 0.1f, 0.107f, 1.0);

	glUniformMatrix4fv(this->Program->uniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(this->Program->uniform("view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 model_mat;
	model_mat = glm::translate(model_mat, glm::vec3(.0f, -18.2f, .0f)); // Translate it down a bit so it's at the center of the scene
	model_mat = camera.GetRotateMatrix(model_mat);
	glUniformMatrix4fv(this->Program->uniform("model"), 1, GL_FALSE, glm::value_ptr(model_mat));
	glUniformMatrix3fv(this->Program->uniform("normMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(view * model_mat)))));
}


void Effects::loaduniforms_pool()
{
	time_t t = time(0);
	t %= 60;
	float Time = 1.0*(float)t;
	glUniform1f(this->Program->uniform("Time"), Time);
	// Transformation matrices
	glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / screenHeight, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();//glm::lookAt(camera.Position, center, camera.Up);

	//右手定则！！！！！
	// Set lights properties
	glUniform4f(this->Program->uniform("lightPos"), 1.0f, 1.0f, 1.0f, 1.0f);//平行光


	glUniformMatrix4fv(this->Program->uniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(this->Program->uniform("view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 model_mat;
	model_mat = glm::translate(model_mat, glm::vec3(.0f, -18.2f, .0f)); // Translate it down a bit so it's at the center of the scene
	model_mat = camera.GetRotateMatrix(model_mat);
	glUniformMatrix4fv(this->Program->uniform("model"), 1, GL_FALSE, glm::value_ptr(model_mat));
	//glUniformMatrix4fv(this->Program->uniform("model_inv"), 1, GL_FALSE, glm::value_ptr(glm::inverse(model_mat)));
	glUniformMatrix3fv(this->Program->uniform("normMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(view * model_mat)))));

	int num = textures.size() + 1;
	glActiveTexture(GL_TEXTURE0 + num);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glUniform1i(Program->uniform("texture0"), num);
	glActiveTexture(GL_TEXTURE0);
}


void Effects::loaduniforms_envmap()
{
	// Transformation matrices
	glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 1.0f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();//glm::lookAt(camera.Position, center, camera.Up);
	//右手定则！！！！！
	// Set lights properties
	glUniform4f(this->Program->uniform("lightPos"), 1.0f, 1.0f, 1.0f, 1.0f);//平行光

	glUniform1f(Program->uniform("materialSpecularExponent"), 0.05f);
	glUniform4f(Program->uniform("materialSpecularColor"), 0.1f, 0.1f, 0.1f, 1.0);
	
	glm::mat4 model_mat;
	model_mat = glm::translate(model_mat, glm::vec3(.0f, -18.2f, .0f)); // Translate it down a bit so it's at the center of the scene
	model_mat = camera.GetRotateMatrix(model_mat);
	
	glUniformMatrix4fv(this->Program->uniform("MVPmat"), 1, GL_FALSE, glm::value_ptr(projection*view*model_mat));
	glUniformMatrix4fv(this->Program->uniform("MVmat"), 1, GL_FALSE, glm::value_ptr(view*model_mat));
	glUniformMatrix3fv(this->Program->uniform("normMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(view * model_mat)))));

	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture);
	//glUniform1i(Program->uniform("texture0"), 0);
}


void Effects::loaduniforms_cloud()
{
	// Transformation matrices
	glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 1.0f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();//glm::lookAt(camera.Position, center, camera.Up);
	
	time_t t = time(0);
	t %= 60;
	float Time = 1.0*(float)t;
	glUniform1f(this->Program->uniform("Time"), Time);

	//glUniform4f(Program->uniform("materialDiffuseColor0"), 0.1f, 0.8f, 0.3f,1.0f);
	//glUniform4f(Program->uniform("materialDiffuseColor1"), 1.0f, 1.0f, 1.0f, 1.0);

	glm::mat4 model_mat;
	model_mat = glm::translate(model_mat, glm::vec3(.0f, -18.2f, .0f)); // Translate it down a bit so it's at the center of the scene
	model_mat = camera.GetRotateMatrix(model_mat);

	glUniformMatrix4fv(this->Program->uniform("MVPmat"), 1, GL_FALSE, glm::value_ptr(projection*view*model_mat));
	glUniformMatrix4fv(this->Program->uniform("MVmat"), 1, GL_FALSE, glm::value_ptr(view*model_mat));
	glUniformMatrix3fv(this->Program->uniform("normMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(view * model_mat)))));
}


void Effects::load_particle_texture()
{
	loadSingleTexture("bg.jpg", 0);
}


void Effects::PARTICLEsetUp()
{
	GLSLShaderPtr vertShader = std::make_shared<GLSLShader>();
	GLSLShaderPtr fragShader = std::make_shared<GLSLShader>();

	vertShader->create(GL_VERTEX_SHADER);
	vertShader->loadFromFile("particle.vert");
	vertShader->compile();

	fragShader->create(GL_FRAGMENT_SHADER);
	fragShader->loadFromFile("particle.frag");
	fragShader->compile();

	particleProgram = std::make_shared<GLSLProgram>();
	particleProgram->create();

	particleProgram->attach(vertShader);
	particleProgram->attach(fragShader);

	particleProgram->bindAttribute("lifetime", ATTRIBUTE_LIFETIME_LOCATION);
	particleProgram->bindAttribute("startPos", ATTRIBUTE_STARTPOSITION_LOCATION);
	particleProgram->bindAttribute("endPos", ATTRIBUTE_ENDPOSITION_LOCATION);

	particleProgram->link();

	particleProgram->detach(vertShader);
	particleProgram->detach(fragShader);
}


GLuint Effects::loadSingleTextureFromMat(cv::Mat image)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, facetex);

	if (image.channels() == 4)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.cols, image.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
		
	}
	else if (image.channels() == 3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
		
	}
	else
		cout << image.channels() << endl;

	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//SOIL_free_image_data(image);
	//delete[]img;
	return facetex;
}


void Effects::loaduniforms_makeup_test()
{
	time_t t = time(0);
	t %= 60;
	float Time = 1.0*(float)t;
	glUniform1f(this->Program->uniform("Time"), Time);
	// Transformation matrices
	glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / screenHeight, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();//glm::lookAt(camera.Position, center, camera.Up);

	//右手定则！！！！！
	glUniform3f(this->Program->uniform("viewPos"), 0.f, 0.f, 30.f);

	// Set lights properties
	glUniform4f(this->Program->uniform("lightPos"), 1.0f, 1.0f, 0.0f, 0.0f);//平行光

	glUniform1f(Program->uniform("materialSpecularExponent"), 0.1f);
	glUniform4f(Program->uniform("materialSpecularColor"), 0.4f, 0.1f, 0.107f, 1.0);

	glUniformMatrix4fv(this->Program->uniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(this->Program->uniform("view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 model_mat;
	model_mat = glm::translate(model_mat, glm::vec3(.0f, -18.2f, .0f)); // Translate it down a bit so it's at the center of the scene
	model_mat = camera.GetRotateMatrix(model_mat);
	glUniformMatrix4fv(this->Program->uniform("model"), 1, GL_FALSE, glm::value_ptr(model_mat));
	glUniformMatrix3fv(this->Program->uniform("normMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(view * model_mat)))));
	glUniformMatrix4fv(this->Program->uniform("modelViewProjection"), 1, GL_FALSE, glm::value_ptr(projection*view*model_mat));
	glUniformMatrix4fv(this->Program->uniform("MVmat"), 1, GL_FALSE, glm::value_ptr(view*model_mat));
}
