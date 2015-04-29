#version 150 core

in vec3 position;
in vec4 vcolor;

out vec4 color;
out vec2 texcoord;

void main() {
    
    color = vec4(vcolor);
    gl_Position = vec4(position, 1.0);
}
