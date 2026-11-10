#pragma once
#pragma warning(disable : 4018)

#include <GL/glew.h>
#include <Engine/stb_image.h>
#include <iostream>
#include <vector>


class CTerrain
{
public:

	CTerrain(const char* filename);
	~CTerrain();

	void DrawTerrain();


private:

	int numStrips;
	int numTrisPerStrip;

	GLuint terrainVAO, terrainVBO, terrainEBO;
};

