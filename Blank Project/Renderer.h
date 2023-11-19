#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Light.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
#include "Snow.h"
#include "Sun.h"

class HeightMap;
class Camera;
class Shader;

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	~Renderer(void);

	void RenderScene() override;
	void RenderScene_blu();
	void UpdateScene(float dt, int op);
	void UpdateSkyBox(int op);
	void AutoUpdateCamera2(float dt);
	void AutoUpdateCamera3(float dt);
	void AutoUpdateCamera4(float dt);
	void UpdateSkyCamera(float dt);

	void RenderSceneNight();

public :
	void DrawHeightMap();
	void DrawSmallHeightMap();
	void DrawWater();
	void DrawSkybox();
	void DrawSun();

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes(Camera* camera);
	void DrawNode(Camera* camera, SceneNode* n);



	

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;

	SceneNode* root;
	SceneNode* npcNode;
	SceneNode* sun;
	SceneNode* ear;
	SceneNode* atmos;

	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* npcShader;
	Shader* sceneShader;
	Shader*  sunShader;


	HeightMap* heightMap;
	Mesh* quad;
	Mesh* npc;
	Mesh* sphere;
	Mesh* sphere_sun;
	Mesh* sphere_ear;


	Light* light;
	Light* pointLights;
	Camera* camera;
	Camera* skycamera;
	

	GLuint cubeMap;
	GLuint cubeMap2;
	GLuint cubeMap3;
	GLuint waterTex;
	GLuint earthTex;
	GLuint earthBump;
	GLuint nodeTex;
	GLuint treeTex;
	GLuint sunTexture;
	GLuint earthTexture;
	GLuint atmosTexture;

	float waterRotate;
	float waterCycle;


	int currentFrame;
	float frameTime;

	MeshAnimation* anim;
	MeshMaterial* material;
	vector <GLuint > matTextures;

	Snow* snow;

	void DrawScene();
	void DrawPointLights();
	void CombineBuffers();
	void GenerateScreenTexture(GLuint& into, bool depth = false);
	void DrawHeightmapNoLight();
	Matrix4 defaultprojMatrix;
	GLuint bufferFBO;
	GLuint bufferDepthTex;
	GLuint bufferNormalTex;
	GLuint pointLightFBO;
	GLuint lightDiffuseTex;
	GLuint lightSpecularTex;
	Shader* heightmapNolightShader;
	Shader* pointlightShader;
	Shader* combineShader;
	GLuint bufferColourTex[2];



	void DrawPostProcess();
	void PresentScene();
	Shader* processShader;
	GLuint processFBO;


};


