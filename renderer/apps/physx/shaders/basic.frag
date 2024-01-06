#version 410 core

layout (location=0) out vec4 frag_colour;

in vec3 frag_normal;
in vec3 frag_position;
in vec3 colour;
in vec3 norm;

uniform vec3 light_dir;

void main() {
    vec3 n_n           = normalize(norm);
    vec3 n_ld          = normalize(-light_dir);

    // Calc diffuse lighting geometry component
    float n_dot_w      = max(0, dot(n_ld, n_n));

    vec3 ambient_light = 0.8 * 1.0 * colour;
    vec3 diff_light    = 0.4 * n_dot_w * 1.0 * colour;
    vec3 l1 = diff_light + ambient_light;

    frag_colour = vec4(l1, 1.0);
}