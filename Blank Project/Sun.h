#pragma once
#include "../nclgl/SceneNode.h"
#include "../nclgl/OGLRenderer.h"

class Sun : public SceneNode {
public:
    Sun();
    void Update(float dt) override;

protected:
    float radius = 500.0f; // Adjust the size of the sun
    // GLuint Texture; // Texture ID for the sun


     // Add any additional variables you need here
};