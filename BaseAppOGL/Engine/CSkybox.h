#pragma once
#include <windows.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <Engine/stb_image.h>
#include <iostream>

class CSkybox
{
public:

	CSkybox(std::vector<std::string> faces);
	~CSkybox();

	void DrawSkybox();
	GLuint GetSkyboxTexture();	

private:
	GLuint cubemapTexture;

	GLuint skyboxVAO;
	GLuint skyboxVBO;
	GLuint skyboxEBO;
	
	float skyboxVertices[24] =
	{
		//   Coordinates
		-1.0f, -1.0f,  1.0f,//        7--------6
		 1.0f, -1.0f,  1.0f,//       /|       /|
		 1.0f, -1.0f, -1.0f,//      4--------5 |
		-1.0f, -1.0f, -1.0f,//      | |      | |
		-1.0f,  1.0f,  1.0f,//      | 3------|-2
		 1.0f,  1.0f,  1.0f,//      |/       |/
		 1.0f,  1.0f, -1.0f,//      0--------1
		-1.0f,  1.0f, -1.0f
	};

	GLuint skyboxIndices[36] =
	{
		// Right
		2, 1, 5,
		2, 5, 6,
		// Left
		0, 3, 7,
		0, 7, 4,
		// Top
		5, 4, 7,
		5, 7, 6,
		// Bottom
		0, 1, 2,
		0, 2, 3,
		// Back
		1, 0, 4,
		1, 4, 5,
		// Front
		3, 2, 6,
		3, 6, 7
	};
};

