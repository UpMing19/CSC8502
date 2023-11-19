#pragma once
#include "../nclgl/SceneNode.h"
/*
 This code is adapted from https://github.com/XiangLi-CS/Fantasy-Island/Blank Project/Snow.h
 Author: XiangLi
 License: MIT License
*/
struct Particle {
	Vector3 speed;
	Vector3 position;
	bool isAlive;
};

class Snow : public SceneNode {
public:
	Snow();
	void Update(float dt) override;
protected:
	float variant = 100;
	float base = 100;
	float numParticles = 350;
	vector<Particle> particle_list;
};