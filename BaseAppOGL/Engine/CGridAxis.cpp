#include "CGridAxis.h"

CGridAxis::CGridAxis(float size)
{
	BuildGrid(size);
	BuildAxis(size);
}

CGridAxis::~CGridAxis()
{
    glDeleteVertexArrays(1, &gridVAO);
    glDeleteVertexArrays(1, &axisVAO);
    glDeleteBuffers(1, &gridVBO);
    glDeleteBuffers(1, &axisVBO);
}

void CGridAxis::DrawGrid()
{
    glBindVertexArray(gridVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void CGridAxis::DrawAxis()
{
    glBindVertexArray(axisVAO);
        glDrawArrays(GL_LINES, 0, 6);   // 3 axes × 2 vertices
    glBindVertexArray(0);
}


// Large flat XZ grid quad
void CGridAxis::BuildGrid(float halfSize = 50.f)
{
    float v[] = {
        -halfSize, 0.f,  halfSize,
         halfSize, 0.f,  halfSize,
         halfSize, 0.f, -halfSize,
         halfSize, 0.f, -halfSize,
        -halfSize, 0.f, -halfSize,
        -halfSize, 0.f,  halfSize,
    };
    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}

//  VAO for the three world-space axis lines
void CGridAxis::BuildAxis(float halfLen = 50.f)
{
    // layout: position (x,y,z)  +  color (r,g,b)  — interleaved
    float v[] = {
        // X axis — red
        -halfLen, 0.f, 0.f,   1.f, 0.f, 0.f,
         halfLen, 0.f, 0.f,   1.f, 0.f, 0.f,

         // Y axis — green  (vertical, from -halfLen to +halfLen)
         0.f, -halfLen, 0.f,   0.f, 1.f, 0.f,
         0.f,  halfLen, 0.f,   0.f, 1.f, 0.f,

         // Z axis — blue
         0.f, 0.f, -halfLen,   0.f, 0.f, 1.f,
         0.f, 0.f,  halfLen,   0.f, 0.f, 1.f,
    };

    glGenVertexArrays(1, &axisVAO);
    glGenBuffers(1, &axisVBO);
    glBindVertexArray(axisVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);

    const GLsizei stride = 6 * sizeof(float);

    // attribute 0 — position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

    // attribute 1 — color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
}
