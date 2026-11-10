#pragma once

#include <GL/glew.h>


class CScreenQuad
{
public:
	CScreenQuad();
	~CScreenQuad();
	
	void RenderScreenQuad();

private:

	GLuint screenQuadVAO;
};