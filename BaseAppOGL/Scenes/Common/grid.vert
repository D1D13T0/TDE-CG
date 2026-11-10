#version 330 core
// ─────────────────────────────────────────────────────────────────────────────
//  grid.vert  —  Large flat XZ quad for the infinite grid
// ─────────────────────────────────────────────────────────────────────────────

layout(location = 0) in vec3 aPos;

uniform mat4 uView;
uniform mat4 uProj;

out vec3 vWorldPos;

void main()
{
    vWorldPos   = aPos;
    gl_Position = uProj * uView * vec4(aPos, 1.0);
}
