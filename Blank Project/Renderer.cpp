#include "Renderer.h"
#include <algorithm>
#include "../nclgl/Light.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Shader.h"


const int LIGHT_NUM = 32;


Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{
	quad = Mesh::GenerateQuad();
	sphere = Mesh::LoadFromMeshFile("Sphere.msh");
	sphere_sun = Mesh::LoadFromMeshFile("Sphere.msh");
	sphere_ear = Mesh::LoadFromMeshFile("Sphere.msh");
	heightMap = new HeightMap(TEXTUREDIR"noise3.jpg");
	npc = Mesh::LoadFromMeshFile("Role_T.msh");



	//Anim Frame
	currentFrame = 0;
	frameTime = 0.0f;

	atmosTexture = SOIL_load_OGL_texture(TEXTUREDIR"atmos.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	treeTex = SOIL_load_OGL_texture(TEXTUREDIR"diffuse.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	waterTex = SOIL_load_OGL_texture(TEXTUREDIR"water.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthTex = SOIL_load_OGL_texture(TEXTUREDIR"aerial_rocks_01_diff_4k.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthBump = SOIL_load_OGL_texture(TEXTUREDIR"ground_grey_diff_4k.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	sunTexture = SOIL_load_OGL_texture(TEXTUREDIR"sun.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthTexture = SOIL_load_OGL_texture(TEXTUREDIR"earth.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	cubeMap2 = SOIL_load_OGL_cubemap(TEXTUREDIR"lagoon_ft.tga", TEXTUREDIR"lagoon_bk.tga", TEXTUREDIR"lagoon_up.tga", TEXTUREDIR"lagoon_dn.tga", TEXTUREDIR"lagoon_rt.tga", TEXTUREDIR"lagoon_lf.tga", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"nebulae_ft.jpg", TEXTUREDIR"nebulae_bk.jpg", TEXTUREDIR"nebulae_up.jpg", TEXTUREDIR"nebulae_dn.jpg", TEXTUREDIR"nebulae_rt.jpg", TEXTUREDIR"nebulae_lf.jpg", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	cubeMap3 = cubeMap;

	if (!earthTex || !earthBump || !cubeMap || !waterTex || !treeTex||!sunTexture || !earthTexture)
		return;

	SetTextureRepeating(treeTex, true);
	SetTextureRepeating(earthTex, true);
	SetTextureRepeating(earthBump, true);
	SetTextureRepeating(waterTex, true);
	SetTextureRepeating(earthTexture, true);
	SetTextureRepeating(sunTexture, true);
	SetTextureRepeating(atmosTexture, true);

	

	processShader = new Shader("TexturedVertex.glsl", "processfrag.glsl");
	reflectShader = new Shader("ReflectVertex.glsl", "ReflectFragment.glsl");
	skyboxShader = new Shader("SkyboxVertex.glsl", "SkyboxFragment.glsl");
	lightShader = new Shader("PerPixelVertex.glsl", "PerPixelFragment.glsl");
	npcShader = new Shader("SkinningVertex.glsl", "TexturedFragment.glsl");
	sceneShader = new Shader("SceneVertex.glsl", "SceneFragment.glsl");
	sunShader = new Shader("SceneVertex.glsl", "SceneFragment.glsl");
	heightmapNolightShader = new Shader("BumpVertex.glsl", "bufferFragment.glsl");
	pointlightShader = new Shader("pointlightvert.glsl", "pointlightfrag.glsl");
	combineShader = new Shader(	"combinevert.glsl", "combinefrag.glsl");

	if (!processShader->LoadSuccess() ||!sunShader->LoadSuccess() || !heightmapNolightShader->LoadSuccess() || !pointlightShader->LoadSuccess()|| !combineShader->LoadSuccess())
		return;
	
	if (!reflectShader->LoadSuccess() || !skyboxShader->LoadSuccess() || !lightShader->LoadSuccess() || !npcShader->LoadSuccess() || !sceneShader->LoadSuccess())
		return;

	//Animation
	anim = new MeshAnimation("Role_T.anm");
	material = new MeshMaterial("Role_T.mat");

	for (int i = 0; i < npc->GetSubMeshCount(); ++i)
	{
		const MeshMaterialEntry* matEntry =
			material->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		matTextures.emplace_back(texID);
	}


	Vector3 heightMapSize = heightMap->GetHeightMapSize();


	root = new SceneNode();

	snow = new Snow();
	root->AddChild(snow);


	atmos = new SceneNode();
	atmos->SetMesh(sphere_ear);
	atmos->SetTransform(Matrix4::Translation(heightMapSize * Vector3(0.5f, 0.5f, 0.5f)));
	atmos->SetTexture(atmosTexture);
	atmos->SetModelScale(Vector3(2000.0f, 1000.0f, 2000.0f));
	atmos->SetColour(Vector4(1.0f, 1.0f, 1.0f, 0.5f));
	atmos->SetBoundingRadius(500.0f);
	root->AddChild(atmos);

	sun = new SceneNode();
	sun->SetMesh(sphere_sun);
	sun->SetTransform(Matrix4::Translation(Vector3(400.0f, 1000.0f, 100.0f)));
	sun->SetTexture(sunTexture);
	sun->SetModelScale(Vector3(500.0f, 500.0f, 500.0f));
	sun->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	sun->SetBoundingRadius(500.0f);
	root->AddChild(sun);


	ear = new SceneNode();
	ear->SetMesh(sphere_sun);
	ear->SetTransform(Matrix4::Translation(Vector3(1000.0f, 1000.0f, 1000.0f)));
	ear->SetTexture(earthTexture);
	ear->SetModelScale(Vector3(250.0f, 250.0f, 250.0f));
	ear->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	ear->SetBoundingRadius(500.0f);
	root->AddChild(ear);

	npcNode = new SceneNode();
	npcNode->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	npcNode->SetTransform(Matrix4::Translation(heightMapSize * Vector3(0.6f, 0.9f, -0.15f)));
	npcNode->SetModelScale(Vector3(200.0f, 200.f, 200.0f));
	npcNode->SetMesh(npc);
	npcNode->SetAniTexture(matTextures);
	npcNode->SetAnimation(anim);
	root->AddChild(npcNode);


	camera = new Camera(-13.460030f, 320.0f, 0.0f, heightMapSize * Vector3(0.5f, 0.5f, 0.5f));

	camera->SetPosition(heightMapSize * Vector3(0.6f, 1.6f, 0.2f));
	camera->SetPosition( Vector3(-117.380478, 553.504822, 3040.788818));
	camera->SetPitch(-10.460030f);
	camera->SetYaw( 320.0);
	skycamera = new Camera(-44.0f, 303.0f, 0.0f, heightMapSize * Vector3(1432.0f, 765000.0f, 1432.0f));
	skycamera->SetPosition(Vector3(-1799.0f, 4140.0f, 3504.0f));

	light = new Light(heightMapSize * Vector3(0.5f, 0.5f, 0.5f), Vector4(1, 1, 1, 1), 30 * heightMapSize.x);

	light->SetColour(Vector4(1.5f, 1.5f, 1.5f, 1.0f));
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);


	pointLights = new Light[LIGHT_NUM];
	for (int i = 0; i < LIGHT_NUM; i++) {
		Light& l = pointLights[i];
		l.SetPosition(Vector3(rand() % (int)heightMapSize.x, 20.0f,rand() % (int)heightMapSize.z));
		l.SetColour(Vector4(0.5f + (float)(rand() / (float)RAND_MAX),0.5f + (float)(rand() / (float)RAND_MAX), 0.5f + (float)(rand() / (float)RAND_MAX),	 1));
		l.SetRadius(250.0f + (rand() % 250));
	}

	defaultprojMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);
	projMatrix = defaultprojMatrix;


	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height,
		0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);


	for (int i = 0; i < 2; ++i)
	{
		glGenTextures(1, &bufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &processFBO);
	glGenFramebuffers(1, &pointLightFBO);


	GLenum buffers[2] = {
	GL_COLOR_ATTACHMENT0,
	GL_COLOR_ATTACHMENT1
	};

	GenerateScreenTexture(bufferDepthTex, true);
	if (!bufferDepthTex)return;
	

	GenerateScreenTexture(bufferNormalTex);
	if (!bufferNormalTex)return;
	
	GenerateScreenTexture(lightDiffuseTex);
	if (!lightDiffuseTex)	return;
	
	GenerateScreenTexture(lightSpecularTex);
	if (!lightSpecularTex)	return;
	


	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,	GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,	GL_TEXTURE_2D, bufferColourTex[0], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,	GL_TEXTURE_2D, bufferNormalTex, 0);
	glDrawBuffers(2, buffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=GL_FRAMEBUFFER_COMPLETE)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, lightDiffuseTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0])return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	waterRotate = 0.0f;
	waterCycle = 0.0f;

	init = true;

}
Renderer::~Renderer(void)
{
}

void Renderer::UpdateSkyBox(int op) {
	if (op == 1) cubeMap = cubeMap2;
	else cubeMap = cubeMap3;
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
}
//Camera
void Renderer::UpdateScene(float dt,int op)
{
	if (op == 1) camera->UpdateCamera(dt);
	else if (op == 2) camera->AutoUpdateCamera(heightMap->GetHeightMapSize(), dt);
	else if (op == 3) AutoUpdateCamera3(dt);
	else if (op == 4) AutoUpdateCamera4(dt);
	viewMatrix = camera->BuildViewMatrix();
	waterRotate += dt * 2.0f; 
	waterCycle += dt * 0.25f; 


	frameTime -= dt;
	while (frameTime < 0.0f)
	{
		currentFrame = (currentFrame + 1) % anim->GetFrameCount();
		frameTime += 1.0f / anim->GetFrameRate();
	}

	npcNode->SetCurrentFrame(currentFrame);
	sun->SetCurrentFrame(currentFrame);
	ear->SetCurrentFrame(currentFrame);

	{
		float npcSpeed = 100.0f;
		Matrix4 targetPosition = Matrix4::Translation(heightMap->GetHeightMapSize() * Vector3(0.6f, 0.9f, 1.0f));
		Matrix4 originalPosition = npcNode->GetTransform();
		Matrix4 currentTarget = targetPosition;
		Matrix4 npcDirection = (currentTarget - originalPosition).Normalised(); 
		bool isMovingTowardsTarget = true; 
		Matrix4 currentPosition = npcNode->GetTransform();
		Matrix4 displacement = npcDirection * npcSpeed * dt;
		Matrix4 newPosition = currentPosition + displacement;
		if ( (newPosition - currentTarget).Length() <= npcSpeed * dt)
		{
			newPosition = Matrix4::Translation(heightMap->GetHeightMapSize() * Vector3(0.6f, 0.9f, -0.15f));
			npcDirection = (originalPosition - currentTarget).Normalised();
			isMovingTowardsTarget = false;
		}
		npcNode->SetTransform(newPosition);

	}

	{
		static float rotation = 0.0f; 
		float rotationSpeed = 10.0f;
		rotation += rotationSpeed * dt;	
		if (rotation >= 360.0f) {
			rotation -= 360.0f;
		}
		Matrix4 rotationMatrix = Matrix4::Rotation(rotation, Vector3(0.0f, 1.0f, 0.0f));
		sun->SetTransform(sun->GetTransform() * rotationMatrix);
	}

	{
		float rotationSpeed = 0.01f;
		static float rotation = 0.0f; 
		Vector3 rotationCenter = Vector3(400.0f, 1000.0f, 100.0f);
		rotation += rotationSpeed * dt;

		while (rotation >= 360.0f) {
			rotation -= 360.0f;
		}

		Matrix4 rotationMatrix = Matrix4::Rotation(rotation, Vector3(0.0f, 1.0f, 0.0f));
		Vector3 toSun = ear->GetTransform().GetPositionVector() - rotationCenter;
		Matrix4 translationToCenter = Matrix4::Translation(-rotationCenter);
		Matrix4 rotationAtCenter = translationToCenter * rotationMatrix;
		Matrix4 translationBack = Matrix4::Translation(rotationCenter);
		Matrix4 finalTransform = translationBack * rotationAtCenter * ear->GetTransform();
		ear->SetTransform(finalTransform);

	}
	

	//Scene root
	root->Update(dt);
}


void LockCameraToSceneNode(Camera& camera,  SceneNode  *node)
{

}
void Renderer::AutoUpdateCamera3(float dt) {

	Vector3 v = npcNode->GetTransform().GetPositionVector();
	v.y += 480.f;
	v.z -= 400.0f;
	camera->SetPosition(v);
	camera->SetPitch(-18.779964f);
	camera->SetYaw(183.0f);
}
void Renderer::AutoUpdateCamera4(float dt) {

	camera->SetPitch(-33.0f);
	camera->SetYaw(323.0f);
	camera->SetPosition(Vector3(-1799.0f, 4140.0f, 3504.0f));

}

void Renderer::UpdateSkyCamera(float dt) {
	float pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);
	float yaw = skycamera->GetYaw();
	if (yaw < 0)
	{
		yaw += 360.0f;
	}
	if (yaw > 360.0f)
	{
		yaw -= 360.0f;
	}

	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));

	Vector3 forward = rotation * Vector3(0, 0, -1);
	Vector3 right = rotation * Vector3(1, 0, 0);

	float forwardspeed = 10.0f * dt;

	float rightspeed = 10.f * dt;
	skycamera->SetPosition(skycamera->GetPosition() + forward * forwardspeed + right * rightspeed);

}


void Renderer::RenderScene()
{

	BuildNodeLists(root);
	SortNodeLists();

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width, height);

	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);

	DrawSkybox();
	DrawHeightMap();
	DrawWater();
	DrawNodes(camera);


	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0.75 * width, 0.66 * height, (width / height) * width / 3, (width / height) * height / 3);
		
    viewMatrix = skycamera->BuildViewMatrix();

	DrawSkybox();
	DrawHeightMap();
	DrawWater();
	DrawNodes(camera);

	
	ClearNodeLists();
}

void Renderer::RenderScene_blu()
{

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	BindShader(sceneShader);
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTex);
	heightMap->Draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	DrawPostProcess();
	PresentScene();

}
void Renderer::RenderSceneNight()
{
	BuildNodeLists(root);
	SortNodeLists();

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glViewport(0, 0, width, height);
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);
	DrawScene();
	DrawPointLights();
	CombineBuffers();

	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0.75 * width, 0.66 * height, (width / height) * width / 3, (width / height) * height / 3);
	viewMatrix = skycamera->BuildViewMatrix();

	DrawScene();

	ClearNodeLists();


}


void Renderer::DrawScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);

	DrawSkybox();
	DrawHeightmapNoLight();
	DrawNodes(camera);
	DrawWater();


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Renderer::DrawHeightmapNoLight()
{
	BindShader(heightmapNolightShader);

	glUniform1i(glGetUniformLocation(heightmapNolightShader->GetProgram(),
		"diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTex);

	glUniform1i(glGetUniformLocation(heightmapNolightShader->GetProgram(),
		"bumpTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, earthBump);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = defaultprojMatrix;

	UpdateShaderMatrices();

	heightMap->Draw();
}
void Renderer::DrawPointLights()
{
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	BindShader(pointlightShader);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_ONE, GL_ONE);
	glCullFace(GL_FRONT);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);

	glUniform1i(glGetUniformLocation(pointlightShader->GetProgram(), "depthTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glUniform1i(glGetUniformLocation(pointlightShader->GetProgram(), "normTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glUniform3fv(glGetUniformLocation(pointlightShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform2f(glGetUniformLocation(pointlightShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(glGetUniformLocation(pointlightShader->GetProgram(), "inverseProjView"), 1, false, invViewProj.values);
	UpdateShaderMatrices();
	for (int i = 0; i < 40; ++i) {
		Light& l = pointLights[i];
		SetShaderLight(l);
		sphere->Draw();
	}
	sphere->Draw();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);

	glDepthMask(GL_TRUE);

	glClearColor(0.2f, 0.2f, 0.2f, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Renderer::CombineBuffers()
{
	BindShader(combineShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "diffuseLight"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lightDiffuseTex);

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "specularLight"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);

	quad->Draw();
}




void Renderer::DrawSkybox()
{
	glDepthMask(GL_FALSE);

	BindShader(skyboxShader);
	UpdateShaderMatrices();

	quad->Draw();

	glDepthMask(GL_TRUE);
}
void Renderer::DrawHeightMap()
{
	BindShader(lightShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(lightShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTex);

	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "bumpTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, earthBump);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	heightMap->Draw();
}

void Renderer::DrawSmallHeightMap()
{

	BindShader(lightShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(lightShader->GetProgram(), "cameraPos"), 1, (float*)&skycamera->GetPosition());

	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTex);

	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "bumpTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, earthBump);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	heightMap->Draw();

}
void Renderer::DrawWater()
{
	BindShader(reflectShader);

	glUniform3fv(glGetUniformLocation(reflectShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterTex);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	Vector3 hSize = heightMap->GetHeightMapSize();

	Vector3 newHeight = hSize * 0.5f;
	newHeight.y += 80.0f;
	modelMatrix = Matrix4::Translation(newHeight) * Matrix4::Scale(hSize * 0.5f) * Matrix4::Rotation(90, Vector3(1, 0, 0));
	textureMatrix = Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) * Matrix4::Scale(Vector3(10, 10, 10)) * Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));

	UpdateShaderMatrices();
	quad->Draw();
}
void Renderer::DrawSun()
{
}


void Renderer::BuildNodeLists(SceneNode* from)
{

	Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
	from->SetCameraDistance(Vector3::Dot(dir, dir));

	if (from->GetColour().w < 1.0f)
	{
		transparentNodeList.push_back(from);
	}
	else
	{
		nodeList.push_back(from);
	}

	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i)
	{
		BuildNodeLists((*i));
	}
}

void Renderer::SortNodeLists()
{
	std::sort(transparentNodeList.rbegin(),
		transparentNodeList.rend(),
		SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(),
		nodeList.end(),
		SceneNode::CompareByCameraDistance);
}

void Renderer::DrawNodes(Camera* camera)
{
	for (const auto& i : nodeList)
	{
		DrawNode(camera, i);
	}
	for (const auto& i : transparentNodeList)
	{
		DrawNode(camera, i);
	}
}

void Renderer::DrawNode(Camera* camera, SceneNode* n)
{
	if (n->GetMesh())
	{

		if (n == npcNode) {

			BindShader(npcShader);

			glUniform1i(glGetUniformLocation(npcShader->GetProgram(), "diffuseTex"), 0);
			UpdateShaderMatrices();

			Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
			glUniformMatrix4fv(glGetUniformLocation(npcShader->GetProgram(), "modelMatrix"), 1, false, model.values);
			glUniform4fv(glGetUniformLocation(npcShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());

			glUniform3fv(glGetUniformLocation(npcShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

			nodeTex = matTextures[0];

			glUniform1i(glGetUniformLocation(npcShader->GetProgram(), "useTexture"), nodeTex);

			vector<Matrix4> frameMatrices;

			const Matrix4* invBindPose = npc->GetInverseBindPose();
			const Matrix4* frameData = anim->GetJointData(currentFrame);

			for (unsigned int i = 0; i < npc->GetJointCount(); ++i)
			{
				frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
			}

			int j = glGetUniformLocation(npcShader->GetProgram(), "joints");
			glUniformMatrix4fv(j, frameMatrices.size(), false,
				(float*)frameMatrices.data());

			for (int i = 0; i < npc->GetSubMeshCount(); ++i)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, matTextures[i]);
				npc->DrawSubMesh(i);
			}
		}
		else if (n == sun) {
			BindShader(sunShader);

			
			glUniform1i(glGetUniformLocation(sunShader->GetProgram(), "diffuseTex"), 0);
			UpdateShaderMatrices();

			Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());

			
			glUniformMatrix4fv(glGetUniformLocation(sunShader->GetProgram(), "modelMatrix"), 1, false, model.values);
			glUniform4fv(glGetUniformLocation(sunShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, sunTexture); 
			glUniform1i(glGetUniformLocation(sunShader->GetProgram(), "useTexture"), 1); 

	
			n->Draw(*this);

			
		}
		else if (n == ear) {
			BindShader(sunShader);


			glUniform1i(glGetUniformLocation(sunShader->GetProgram(), "diffuseTex"), 0);
			UpdateShaderMatrices();

			Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());

			glUniformMatrix4fv(glGetUniformLocation(sunShader->GetProgram(), "modelMatrix"), 1, false, model.values);
			glUniform4fv(glGetUniformLocation(sunShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, earthTexture);
			glUniform1i(glGetUniformLocation(sunShader->GetProgram(), "useTexture"), 1);


			n->Draw(*this);
		}
		else
		{

			BindShader(sceneShader);

			glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex"), 0);
			UpdateShaderMatrices();

			Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
			glUniformMatrix4fv(glGetUniformLocation(sceneShader->GetProgram(), "modelMatrix"), 1, false, model.values);
			glUniform4fv(glGetUniformLocation(sceneShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());

			glUniform3fv(glGetUniformLocation(sceneShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

			treeTex = n->GetTexture();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, treeTex);

			glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "useTexture"), treeTex);

			n->Draw(*this);

		}

	}
}

void Renderer::ClearNodeLists()
{
	transparentNodeList.clear();
	nodeList.clear();
}

void Renderer::GenerateScreenTexture(GLuint& into, bool depth)
{
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint format = depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8;
	GLuint type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, type, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}

const int POST_PASSES = 10;
void Renderer::DrawPostProcess()
{
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(processShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(
		processShader->GetProgram(), "sceneTex"), 0);
	for (int i = 0; i < POST_PASSES; ++i)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, bufferColourTex[1], 0);
		glUniform1i(glGetUniformLocation(
			processShader->GetProgram(), "isVertical"), 0);

		glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
		quad->Draw();
		glUniform1i(glGetUniformLocation(
			processShader->GetProgram(), "isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, bufferColourTex[0], 0);

		glBindTexture(GL_TEXTURE_2D, bufferColourTex[1]);
		quad->Draw();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::PresentScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(sceneShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex"), 0);
	quad->Draw();
}

