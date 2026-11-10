#version 330 core
// ─────────────────────────────────────────────────────────────────────────────
//  axis.frag  —  Outputs the interpolated per-vertex axis color
// ─────────────────────────────────────────────────────────────────────────────

in  vec3 vColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(vColor, 1.0);
}
