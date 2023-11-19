#pragma once
#include "Matrix4.h"
#include "Vector3.h"

class Camera
{
public:
	Camera(void) { yaw = 0.0f; pitch = 0.0f; roll = 0.0f; }
	Camera(float pitch, float yaw, float roll, Vector3 position) { this->pitch = pitch; this->yaw = yaw; this->roll = roll; this->position = position; }
	~Camera(void) {};

	void UpdateCamera(float dt = 1.0f);
	void AutoUpdateCamera(Vector3 heightmapsize, float dt = 1.0f);
	void AutoUpdateCamera2(Vector3 heightmapsize, float dt = 1.0f);
	void AutoUpdateCamera3(Vector3 heightmapsize, float dt = 1.0f);

	Matrix4 BuildViewMatrix();

	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }

	float GetYaw() const { return yaw; }
	void SetYaw(float y) { yaw = y; }

	float GetPitch() const { return pitch; }
	void SetPitch(float p) { pitch = p; }

	float GetRoll() const { return roll; }
	void SetRoll(float r) { roll = r; }


	void SetRotation(float yaw, float pitch, float roll)
	{
		SetYaw(yaw);
		SetPitch(pitch);
		SetRoll(roll);
	}

protected:
	float yaw;
	float pitch;
	float roll;
	Vector3 position;
};