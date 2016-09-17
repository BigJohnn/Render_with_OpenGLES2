#include "stdafx.h"
#include "UserInterface.h"

using namespace std;

void loadTex(UserData *userData)
{
	glGenTextures(1, &userData->particle_tex);

	int width, height, channel;
	unsigned char* image = SOIL_load_image("bg.jpg", &width, &height, &channel, SOIL_LOAD_AUTO);

	// Assign texture to ID
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, userData->particle_tex);

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
}

void UI(UserData *userData)
{
	string objpath = "cap.obj";
	//string objpath = "cardboard_try1.dae";
	//string objpath = "sceneLight.scn";
	//string objpath = "occluder.dae";
#ifdef shadow_debug
	cout << endl << "Shadow Debugging mode!!" << endl;
	userData->effects_name = "shadow";
	userData->effects_enum = shadow;
	userData->effect = new Effects((userData->effects_name + ".vert").c_str(), (userData->effects_name + ".frag").c_str());
	userData->effect->windowInformation(userData->width, userData->height);
	userData->effect->LoadMeshes(objpath);
	userData->effect->DEPTHsetUp();
	FBOsetUp(userData);
	userData->effect->QUADsetUp();
#elif defined env_debug
	userData->effects_enum = envmap;
	userData->effects_name = "envmap";

	userData->effect = new Effects((userData->effects_name + ".vert").c_str(), (userData->effects_name + ".frag").c_str());
	userData->effect->windowInformation(userData->width, userData->height);
	userData->effect->LoadMeshes(objpath);
	userData->effect->loadTextures(envmap);
#else
	cout << "Native Shaders: " << endl;
	cout << "-none" << endl;
	cout << "-fire" << endl;
	cout << "-glass" << endl;
	cout << "-normal(with tangent plane canculation)" << endl;
	cout << "-normalmap" << endl;
	cout << "-phong" << endl;
	cout << "-rainbow" << endl;
	cout << "-toon" << endl;
	cout << "-worble" << endl;
	cout << "-polkadot" << endl;
	cout << "-brick" << endl;
	cout << "-pool" << endl;
	cout << "-tile" << endl;
	cout << "-envmap" << endl;
	cout << "-cloud" << endl;
	cout << "-shadow" << endl;
	cout << "-Makeup" << endl;
	cout << "Please select a shader: " << endl;

	//两种写法是等价的？ 
	userData->name_map["model"] = none;
	userData->name_map.insert(map<string, effects>::value_type("fire", fire));
	userData->name_map.insert(map<string, effects>::value_type("glass", glass));
	userData->name_map["normal"] = normal;
	userData->name_map["normalmap"] = normalmap;
	userData->name_map["phong"] = phong;
	userData->name_map["rainbow"] = rainbow;
	userData->name_map["toon"] = toon;
	userData->name_map["worble"] = worble;
	userData->name_map["polkadot"] = polkadot;
	userData->name_map["brick"] = brick;
	userData->name_map["pool"] = pool;
	userData->name_map["tile"] = tile;
	userData->name_map["envmap"] = envmap;
	userData->name_map["cloud"] = cloud;
	userData->name_map["shadow"] = shadow;
	userData->name_map["Makeup"] = Makeup;

	userData->enum_map[none] = "model";
	userData->enum_map.insert(map<effects, string>::value_type(fire, "fire"));
	userData->enum_map.insert(map<effects, string>::value_type(glass, "glass"));
	userData->enum_map[normal] = "normal";
	userData->enum_map[normalmap] = "normalmap";
	userData->enum_map[phong] = "phong";
	userData->enum_map[rainbow] = "rainbow";
	userData->enum_map[toon] = "toon";
	userData->enum_map[worble] = "worble";
	userData->enum_map[polkadot] = "polkadot";
	userData->enum_map[brick] = "brick";
	userData->enum_map[pool] = "pool";
	userData->enum_map[tile] = "tile";
	userData->enum_map[envmap] = "envmap";
	userData->enum_map[cloud] = "cloud";
	userData->enum_map[shadow] = "shadow";
	userData->enum_map[Makeup] = "Makeup";

	string str;
	cin >> str;

	effects shader = userData->name_map[str];
	userData->effects_enum = shader;
	userData->effects_name = userData->enum_map[shader];

	if (Makeup == userData->effects_enum)
	{
		userData->effect = new Effects((userData->effects_name + ".vsh").c_str(), (userData->effects_name + "AddLightBloom.fsh").c_str());
	}
	else
	{
		userData->effect = new Effects((userData->effects_name + ".vert").c_str(), (userData->effects_name + ".frag").c_str());
	}
	
	userData->effect->windowInformation(userData->width, userData->height);
	userData->effect->LoadMeshes(objpath);
	userData->effect->loadTextures(shader);

	userData->effect->QUADsetUp();
	glGenTextures(1, &userData->effect->facetex);

	cv::Mat I = cv::imread("Argb.png",-1);
	userData->effect->loadSingleTextureFromMat(I);
	//--------particle init----------
	//userData->particles = new Effects(("particle.vert"), ("particle.frag"));
	//userData->particles->windowInformation(userData->width, userData->height);
	//userData->particles->PARTICLEsetUp();
	//loadTex(userData);
	//// Fill in particle data array
	//srand(0);

	//for (int i = 0; i < NUM_PARTICLES; i++)
	//{
	//	float *particleData = &userData->particleData[i * PARTICLE_SIZE];

	//	// Lifetime of particle
	//	(*particleData++) = ((float)(rand() % 10000) / 10000.0f);

	//	// End position of particle
	//	(*particleData++) = ((float)(rand() % 10000) / 5000.0f) - 1.0f;
	//	(*particleData++) = ((float)(rand() % 10000) / 5000.0f) - 1.0f;
	//	(*particleData++) = ((float)(rand() % 10000) / 5000.0f) - 1.0f;

	//	// Start position of particle
	//	(*particleData++) = ((float)(rand() % 10000) / 40000.0f) - 0.125f;
	//	(*particleData++) = ((float)(rand() % 10000) / 40000.0f) - 0.125f;
	//	(*particleData++) = ((float)(rand() % 10000) / 40000.0f) - 0.125f;

	//}

	//glGenBuffers(1, &PVBO);
	//glBindBuffer(GL_ARRAY_BUFFER, PVBO);
	//
	//glBufferData(GL_ARRAY_BUFFER, sizeof(userData->particleData), userData->particleData, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(ATTRIBUTE_LIFETIME_LOCATION);
	//// Load the vertex attributes
	//glVertexAttribPointer(ATTRIBUTE_LIFETIME_LOCATION, 1, GL_FLOAT,
	//	GL_FALSE, PARTICLE_SIZE * sizeof(GLfloat),
	//	(GLvoid*)0);

	//glEnableVertexAttribArray(ATTRIBUTE_ENDPOSITION_LOCATION);
	//glVertexAttribPointer(ATTRIBUTE_ENDPOSITION_LOCATION, 3, GL_FLOAT,
	//	GL_FALSE, PARTICLE_SIZE * sizeof(GLfloat),
	//	(GLvoid*)sizeof(GLfloat));

	//glEnableVertexAttribArray(ATTRIBUTE_STARTPOSITION_LOCATION);
	//glVertexAttribPointer(ATTRIBUTE_STARTPOSITION_LOCATION, 3, GL_FLOAT,
	//	GL_FALSE, PARTICLE_SIZE * sizeof(GLfloat),
	//	(GLvoid*)(sizeof(GLfloat)*4));
	//// Initialize time to cause reset on first update
	//userData->particle_time = 1.0f;
	//--------------------------------------------------


	if (shader == shadow)
	{
		userData->effect->DEPTHsetUp();
		FBOsetUp(userData);
	}
	
#endif
}

//initiate shadow fbo
void FBOsetUp(UserData *data)
{
	glGenFramebuffers(1, &data->depthMapFBO);
	// bind the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, data->depthMapFBO);

	glGenTextures(1, &data->colorTex);
	glGenTextures(1, &data->depthTex);
	glBindTexture(GL_TEXTURE_2D, data->colorTex);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SHADOW_WIDTH, SHADOW_HEIGHT,
		0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
	// bind depth texture and load the texture mip-level 0
	// no texels need to specified as we are going to draw into
	// the texture
	
	glBindTexture(GL_TEXTURE_2D, data->depthTex);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, data->width,
		data->height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT,
		NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	//// specify texture as color attachment
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, data->colorTex, 0);
	// specify texture as depth attachment
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, data->depthTex, 0);


	//------华丽的分割线--.///////\\\\\\\\-.-///////'\\////
	// - Create depth texture
	
	//int main_buffer;
	//glGetIntegerv(GL_FRAMEBUFFER_BINDING, &main_buffer);
	//if (main_buffer < 0) main_buffer = 0;

	//data->depthMapFBO = 0;
	//glGenFramebuffers(1, &data->depthMapFBO);

	//glGenTextures(1, &data->colorTex);
	//glBindTexture(GL_TEXTURE_2D, data->colorTex);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
	//	SHADOW_WIDTH, SHADOW_HEIGHT,
	//	0, GL_LUMINANCE, GL_FLOAT, NULL);

	//glGenRenderbuffers(1, &data->depthMapRBO);
	//glBindRenderbuffer(GL_RENDERBUFFER, data->depthMapRBO);
	//glRenderbufferStorage(GL_RENDERBUFFER,
	//	GL_DEPTH_COMPONENT16,
	//	SHADOW_WIDTH, SHADOW_HEIGHT);

	//glBindFramebuffer(GL_FRAMEBUFFER, data->depthMapFBO);
	//glBindTexture(GL_TEXTURE_2D, data->colorTex);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, data->depthMapRBO);

	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, data->colorTex, 0);

	//// Always check that our framebuffer is ok
	//CheckFramebufferStatus(GL_FRAMEBUFFER);

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
Camera camera(glm::vec3(0.0f, .0f, 30.0f));
GLfloat deltaTime = 0.5f;
GLfloat lastFrame = 0.0f;
// Moves/alters the camera positions based on user input
void Do_Movement(char key)
{
	// Camera controls
	if (key=='w'||key =='W')
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (key=='s'||key =='S')
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (key=='a'||key=='A')
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (key=='d'||key=='D')
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (key=='r' || key =='R')
		camera.Reset();
}

void Process_Mouse(int xpos, int ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void Process_Wheel(int z)
{
	camera.ProcessMouseScroll(z);
}