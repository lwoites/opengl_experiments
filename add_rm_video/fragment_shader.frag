#version 150 core

in vec2 texcoord;
out vec4 outColor;

uniform sampler2D tex_0;

void main()
{
    outColor = texture(tex_0, texcoord);
}