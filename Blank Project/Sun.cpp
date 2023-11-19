#include "Sun.h"

Sun::Sun() {

}



void Sun::Update(float dt) {
  
    float rotationSpeed = 6000.0f; 

  
    float angle = rotationSpeed * dt;

   
    Matrix4 rotationMatrix = Matrix4::Rotation(angle, Vector3(0.0f, 1.0f, 0.0f));


    Vector3 currentPosition = GetWorldTransform().GetPositionVector();


    Vector3 newPosition = rotationMatrix * currentPosition;

  
    SetTransform(Matrix4::Translation(newPosition));

    SceneNode::Update(dt);
}

