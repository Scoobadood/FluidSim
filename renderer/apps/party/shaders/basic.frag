#version 410 core

layout (location=0) out vec4 frag_colour;

in vec3 colour;

uniform vec3 light_dir;

void main() {
    frag_colour = vec4(colour, 1.0);
}