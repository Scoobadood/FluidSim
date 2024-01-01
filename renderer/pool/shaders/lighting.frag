#version 410 core

layout (location=0) out vec4 frag_colour;

in vec3 frag_normal;
in vec3 frag_position;

uniform vec3 object_color;
uniform vec3 light_color;

void main() {
    float ambientStrength = 0.9;
    vec3 ambient = ambientStrength * light_color;
    vec3 result = ambient * object_color;
    frag_colour = vec4(result, 1.0);
}