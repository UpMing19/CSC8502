#pragma once
#include <string>
#include "Mesh.h"

class HeightMap : public Mesh
{
public:
	HeightMap();
	HeightMap(const std::string& name);
	~HeightMap(void) {};

	Vector3 GetHeightMapSize() const { return heightMapSize; }
	int GetHeightAtCoord(int x, int z) const { return vertices[((int)(z/vertexScale.z) * iWidth) + (int)(x/vertexScale.x)].y; }

protected:
	int iWidth;
	int iHeight;
	Vector3 heightMapSize;
	Vector3 vertexScale;
};