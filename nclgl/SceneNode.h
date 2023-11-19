#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include "MeshAnimation.h"
#include "MeshMaterial.h"
#include <vector>

class SceneNode
{
public:



	SceneNode(Mesh* m = NULL, Vector4 colour = Vector4(1, 1, 1, 1));
	~SceneNode(void);

	void SetTransform(const Matrix4 &matrix) { transform = matrix; }
	const Matrix4& GetTransform() const { return transform; }
	Matrix4 GetWorldTransform() const {	return worldTransform; }

	Shader* GetShader() const { return shader; }
	void SetShader(Shader* newShader) { shader = newShader; }

	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 c) { colour = c; }

	Vector3 GetModelScale() const { return modelScale; }
	void SetModelScale(Vector3 s) { modelScale = s; }

	Mesh* GetMesh() const { return mesh; }
	void SetMesh(Mesh* m) { mesh = m; }

	float GetBoundingRadius() const { return boundingRadius; }
	void SetBoundingRadius(float f) { boundingRadius = f; }

	float GetCameraDistance() const { return distanceFromCamera; }
	void SetCameraDistance(float f) { distanceFromCamera = f; }

	void SetTexture(GLuint tex) { texture = tex; }
	GLuint GetTexture() const { return texture; }

	bool IsAnimated() const { return animated; }
	MeshAnimation* GetAnim() const { return anim; }
	MeshMaterial* GetMaterial() const { return material; }
	vector<GLuint> GetMatTextures() const { return matTextures; }
	int GetCurrentFrame() const { return currentFrame; }

	static bool CompareByCameraDistance(SceneNode* a, SceneNode* b) { return (a->distanceFromCamera < b->distanceFromCamera) ? true : false; }

	void AddChild(SceneNode* s);
	virtual void Update(float dt);
	virtual void Draw(const OGLRenderer& r);

	std::vector<SceneNode*>::const_iterator GetChildIteratorStart() { return children.begin(); }
	std::vector<SceneNode*>::const_iterator GetChildIteratorEnd() { return children.end(); }


	void SetAnimation(MeshAnimation* anim) { anim = anim; }
	void SetAniTexture(vector<GLuint> tex) { matTextures = tex; }
	void SetCurrentFrame(int c) { currentFrame = c; }

protected:
	Shader* shader;
	SceneNode* parent;
	Mesh* mesh;
	MeshAnimation* anim;
	MeshMaterial* material;
	vector<GLuint> matTextures;
	Matrix4 worldTransform;
	Matrix4 transform;
	Vector3 modelScale;
	Vector4 colour;
	float boundingRadius;
	float distanceFromCamera;
	GLuint texture;
	std::vector<SceneNode*> children;
	bool animated;
	int currentFrame;
	float frameTime;
};