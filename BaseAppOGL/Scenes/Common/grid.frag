#version 330 core
// ─────────────────────────────────────────────────────────────────────────────
//  grid.frag  —  Infinite XZ world-plane grid (analytical anti-aliasing)
//
//  Reference: Evan Wallace "Antialiased Grid Shader"
//  The mesh is a large flat quad in the XZ plane (Y=0).
//  fwidth() gives us screen-space derivatives for AA grid lines.
// ─────────────────────────────────────────────────────────────────────────────

in  vec3 vWorldPos;
out vec4 FragColor;

uniform float uGridSpacing;     // World-space units between lines (e.g. 1.0)
uniform float uFadeStart;       // Camera distance where grid starts fading
uniform float uFadeEnd;         // Camera distance where grid is invisible
uniform vec3  uCamPos;

void main()
{
    vec2  p    = vWorldPos.xz / uGridSpacing;
    vec2  fw   = fwidth(p);

    // Minor lines
    vec2  grid = abs(fract(p - 0.5) - 0.5) / fw;
    float line = min(grid.x, grid.y);
    float minA = 1.0 - min(line, 1.0);

    // Major lines every 5 cells
    vec2  majP = vWorldPos.xz / (uGridSpacing * 5.0);
    vec2  majG = abs(fract(majP - 0.5) - 0.5) / fwidth(majP);
    float majL = min(majG.x, majG.y);
    float majA = 1.0 - min(majL, 1.0);

    // Axis lines (X = red, Z = blue)
    float axisX = 1.0 - min(abs(vWorldPos.z) / (uGridSpacing * fw.y * 1.5), 1.0);
    float axisZ = 1.0 - min(abs(vWorldPos.x) / (uGridSpacing * fw.x * 1.5), 1.0);

    // Distance fade
    float dist  = length(vWorldPos - uCamPos);
    float fade  = 1.0 - smoothstep(uFadeStart, uFadeEnd, dist);

    vec3  bg    = vec3(0.055, 0.07, 0.10);
    vec3  col   = bg;
    col += vec3(0.10, 0.14, 0.20) * minA  * 0.5;
    col += vec3(0.15, 0.20, 0.30) * majA  * 0.6;
    col += vec3(0.55, 0.15, 0.12) * axisX * 0.9;   // X axis red
    col += vec3(0.12, 0.25, 0.65) * axisZ * 0.9;   // Z axis blue

    float alpha = (minA * 0.4 + majA * 0.5 + axisX * 0.9 + axisZ * 0.9) * fade;
    alpha = clamp(alpha, 0.0, 1.0);

    // Discard fully transparent fragments (floor fill)
    if (alpha < 0.01) discard;

    FragColor = vec4(col, alpha);
}
