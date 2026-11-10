#version 330 core
// ─────────────────────────────────────────────────────────────────────────────
//  axis.vert  —  Simple pass-through for colored axis lines (X, Y, Z)
//
//  Each vertex carries its own color so the three axes can be distinct:
//    X = red  (1,0,0)
//    Y = green(0,1,0)
//    Z = blue (0,0,1)
// ─────────────────────────────────────────────────────────────────────────────

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

uniform mat4 uView;
uniform mat4 uProj;

out vec3 vColor;

void main()
{
    vColor      = aColor;
    gl_Position = uProj * uView * vec4(aPos, 1.0);
}
