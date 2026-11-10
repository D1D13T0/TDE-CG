#version 330 core

in vec2 TexCoord;
in vec3 Normal;
out vec4 FragColor;

uniform sampler2D Texture0;

void main()
{
    vec4 texColor = texture(Texture0, TexCoord);

    texColor.rgb *= texColor.a;
    if(texColor.a < 0.1) // Adjust threshold as needed
        discard;

    FragColor = texColor;
}