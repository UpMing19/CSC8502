#include "../nclgl/window.h"
#include "Renderer.h"

int main() {
	Window w("Cube Mapping!", 1920, 1200, true);
	if (!w.HasInitialised()) {
		return -1;
	}
	int camera = 1;
	int scence = 1;
	int flag = 1;

	Renderer renderer(w);
	if (!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {



		Vector3 cameraPosition = renderer.camera->GetPosition();
		Vector3 cameraOrientation = Vector3(renderer.camera->GetPitch(), renderer.camera->GetYaw(), renderer.camera->GetRoll());

		std::string positionText = "Position: " + std::to_string(cameraPosition.x) + ", "
			+ std::to_string(cameraPosition.y) + ", "
			+ std::to_string(cameraPosition.z);

		std::string orientationText = "Orientation: " + std::to_string(cameraOrientation.x) + ", "
			+ std::to_string(cameraOrientation.y) + ", "
			+ std::to_string(cameraOrientation.z);

		std::cout << positionText << std::endl;
		std::cout << orientationText << std::endl;

		float timestep = w.GetTimer()->GetTimeDeltaSeconds();



		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_DOWN))
		{
			Vector3 vv = renderer.light->GetPosition();
			vv.y -= 100.0f;
			renderer.light->SetPosition(vv);
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_UP))
		{
			Vector3 vv = renderer.light->GetPosition();
			vv.y += 100.0f;
			renderer.light->SetPosition(vv);
		}

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1))
			camera = 1;
		else if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2))
			camera = 2;
		else if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_3))
			camera = 3;
		else if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_4))
			camera = 4;

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_5))
			scence = 1;
		else if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_6))
			scence = 2;
		else if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_7))
			scence = 3;

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_9) && flag)
			renderer.UpdateSkyBox(flag), flag = 0;
		else  if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_9))
			renderer.UpdateSkyBox(flag), flag = 1;

		renderer.UpdateScene(timestep, camera);



		if (scence == 1)	renderer.RenderScene();
		else if (scence == 2) renderer.RenderSceneNight();
		else if (scence == 3) renderer.RenderScene_blu();
		renderer.SwapBuffers();

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
			Shader::ReloadAllShaders();
		}
	}

	return 0;
}
