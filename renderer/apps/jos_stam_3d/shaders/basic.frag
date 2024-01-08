#version 410 core

layout (location=0) out vec4 frag_colour;

uniform sampler2D tx; // Tx

in vec2 frag_tex_coord;

void main() {
    frag_colour = vec4(texture(tx, frag_tex_coord).rgb, 1);
}