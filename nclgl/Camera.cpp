#include "Camera.h"
#include "Window.h"
#include <algorithm>
#include <random>
#include <cmath>

void Camera::UpdateCamera(float dt)
{
	pitch -= (Window::GetMouse()->GetRelativePosition().y);
	yaw -= (Window::GetMouse()->GetRelativePosition().x);

	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);

	if (yaw < 0)
		yaw += 360.0f;
	if (yaw > 360.0f)
		yaw -= 360.0f;

	if (roll < 0)
		roll += 360.0f;
	if (roll > 360.0f)
		roll -= 360.0f;

	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));

	Vector3 forward = rotation * Vector3(0, 0, -1);
	Vector3 right = rotation * Vector3(1, 0, 0);

	float speed = 480.0f * dt;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) 
		position += forward * speed; 
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) 
		position -= forward * speed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) 
		position -= right * speed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) 
		position += right * speed;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT))
		position.y -= speed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE))
		position.y += speed;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_Q))
		roll -= speed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_E))
		roll += speed;
}

void Camera::AutoUpdateCamera(Vector3 heightmapsize, float dt)
{
	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);

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

	float forwardspeed = 100.0f * dt;

	float rightspeed = 10.f * dt;

	position = position + forward * forwardspeed + right * rightspeed;


}

float RandomRange(float min, float max) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(min, max);
	return dis(gen);
}

#define M_PI       3.14159265358979323846
void Camera::AutoUpdateCamera2(Vector3 heightmapsize, float dt)
{
	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);

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

	float forwardspeed = 100.0f * dt;
	float rightspeed = 10.f * dt;

	float randomForwardSpeed = RandomRange(-forwardspeed, forwardspeed);
	float randomRightSpeed = RandomRange(-rightspeed, rightspeed);

	position = position + forward * randomForwardSpeed + right * randomRightSpeed;

	
	float randomYaw = RandomRange(-180.0f, 180.0f);
	float randomPitch = RandomRange(-45.0f, 45.0f);

	yaw += randomYaw * dt;
	pitch += randomPitch * dt;

	
	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);

	
}


Matrix4 Camera::BuildViewMatrix()
{
	return Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix4::Rotation(-roll, Vector3(0, 0, 1)) *
		Matrix4::Translation(-position);
}