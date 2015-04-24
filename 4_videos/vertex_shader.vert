#version 150 core

in vec2 position;
in vec3 vcolor;
in vec2 vtexcoord;
out vec3 color;
out vec2 texcoord;

void main() {
    
    color = vcolor;
    texcoord = vtexcoord;
    gl_Position = vec4(position, 0.0, 1.0);
}
