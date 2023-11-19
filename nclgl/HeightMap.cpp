#include "HeightMap.h"

HeightMap::HeightMap()
{
	iWidth = 512;
	iHeight = 512;
	
	numVertices = iWidth * iHeight;
	numIndices = (iWidth - 1) * (iHeight - 1) * 6;
	vertices = new Vector3[numVertices];
	textureCoords = new Vector2[numVertices];
	indices = new GLuint[numIndices];
	colours = new Vector4[numVertices];

	vertexScale = Vector3(16.0f, 0.5f, 16.0f);
	Vector2 textureScale = Vector2(1 / 16.0f, 1 / 16.0f);

	int* data = new int[(iWidth * iHeight)];
	for (int i = 0; i < iWidth * iHeight; ++i)
		data[i] = 0;

	for (int z = 0; z < iHeight; ++z)
	{
		for (int x = 0; x < iWidth; ++x)
		{
			int offset = (z * iWidth) + x;
			vertices[offset] = Vector3(x*1.0, 1.0*data[offset], z*1.0) * vertexScale;
			textureCoords[offset] = Vector2(x, z) * textureScale;
			colours[offset] = Vector4(data[offset] / 255.0f, data[offset] / 255.0f, data[offset] / 255.0f, 1);
		}
	}

	int i = 0;
	for (int z = 0; z < iHeight - 1; ++z)
	{
		for (int x = 0; x < iWidth - 1; ++x)
		{
			int a = (z * (iWidth)) + x;
			int b = (z * (iWidth)) + (x + 1);
			int c = ((z + 1) * (iWidth)) + (x + 1);
			int d = ((z + 1) * (iWidth)) + x;

			indices[i++] = b;
			indices[i++] = a;
			indices[i++] = c;

			indices[i++] = c;
			indices[i++] = a;
			indices[i++] = d;
		}
	}

	GenerateNormals();
	GenerateTangents();
	BufferData();

	heightMapSize.x = vertexScale.x * (iWidth - 1);
	heightMapSize.y = vertexScale.y * 255.0f;
	heightMapSize.z = vertexScale.z * (iHeight - 1);
}
HeightMap::HeightMap(const std::string& name)
{
	int iChans;
	unsigned char* data = SOIL_load_image(name.c_str(), &iWidth, &iHeight, &iChans, 1);
	if (!data)
	{
		std::cout << "Heightmap can't load file!" << std::endl;
		return;
	}

	numVertices = iWidth * iHeight;
	numIndices = (iWidth - 1) * (iHeight - 1) * 6;
	vertices = new Vector3[numVertices];
	textureCoords = new Vector2[numVertices];
	indices = new GLuint[numIndices];
	colours = new Vector4[numVertices];

	vertexScale = Vector3(16.0f, 1.0f, 16.0f);
	Vector2 textureScale = Vector2(1 / 16.0f, 1 / 16.0f);

	for (int z = 0; z < iHeight; ++z)
	{
		for (int x = 0; x < iWidth; ++x)
		{
			int offset = (z * iWidth) + x;
			vertices[offset] = Vector3(1*x, 2.0*data[offset], 1*z) * vertexScale;
			textureCoords[offset] = Vector2(x, z) * textureScale;
			colours[offset] = Vector4(data[offset] / 255.0f, data[offset] / 255.0f, data[offset] / 255.0f, 1);
		}
	}
	SOIL_free_image_data(data);

	int i = 0;
	for (int z = 0; z < iHeight - 1; ++z)
	{
		for (int x = 0; x < iWidth - 1; ++x)
		{
			int a = (z * (iWidth)) + x;
			int b = (z * (iWidth)) + (x + 1);
			int c = ((z + 1) * (iWidth)) + (x + 1);
			int d = ((z + 1) * (iWidth)) + x;

			indices[i++] = b;
			indices[i++] = a;
			indices[i++] = c;

			indices[i++] = c;
			indices[i++] = a;
			indices[i++] = d;
		}
	}

	GenerateNormals();
	GenerateTangents();
	BufferData();

	heightMapSize.x = vertexScale.x * (iWidth - 1);
	heightMapSize.y = vertexScale.y * 255.0f;
	heightMapSize.z = vertexScale.z * (iHeight - 1);
}