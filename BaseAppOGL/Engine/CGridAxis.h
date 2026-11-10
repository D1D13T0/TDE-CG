#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class CGridAxis
{
	public:
	CGridAxis(float size);
	~CGridAxis();
	void DrawGrid();
	void DrawAxis();

	void BuildGrid(float halfSize);
	void BuildAxis(float halfLen);

	GLuint gridVAO, gridVBO;
	GLuint axisVAO, axisVBO;
};

