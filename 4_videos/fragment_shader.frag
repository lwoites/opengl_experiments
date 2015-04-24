#version 150 core

in vec3 color;
in vec2 texcoord;
out vec4 outColor;

uniform sampler2D tex_0;

void main()
{
    outColor = texture(tex_0, texcoord);
    //outColor = col_1;
    
    //outColor = vec4(color, 1.0);
}