#version 150 core

in vec2 position;
in vec2 vtexcoord;
out vec2 texcoord;

void main() {
    texcoord = vtexcoord;
    gl_Position = vec4(position, 0.0, 1.0);
}
