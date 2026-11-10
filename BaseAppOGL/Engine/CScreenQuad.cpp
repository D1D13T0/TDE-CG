#include "CScreenQuad.h"

CScreenQuad::CScreenQuad()
{
	glGenVertexArrays(1, &screenQuadVAO);
}

CScreenQuad::~CScreenQuad()
{
	glDeleteVertexArrays(1, &screenQuadVAO);
}

void CScreenQuad::RenderScreenQuad()
{
	glBindVertexArray(screenQuadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
}

