#version 410 core

layout (location=0) out vec4 frag_colour;

in vec3 frag_normal;
in vec3 frag_position;
in vec3 colour;

uniform float kd;               // Diffuse constant
uniform float ka;               // Ambient constant
uniform float ks;               // Specular constant
uniform float alpha;            // Shininess of surface

uniform vec3 light_dir;         // From light. Invert for dir to light
uniform float light_intensity;

uniform vec3 object_colour;

void main() {
    vec3 n_n           = normalize(frag_normal);
    vec3 n_ld          = normalize(-light_dir);

    // Calc diffuse lighting geometry component
    float n_dot_w      = max(0, dot(n_ld, n_n));

    // Calc specular lighting Phong
    vec3 look          = -normalize(frag_position);
    vec3 r             = -reflect(n_ld, n_n);
    float r_dot_v      = max(0, dot(r, look));
    float spec_coeff   = pow(r_dot_v, alpha);

    vec3 spec_light    = ks * spec_coeff * light_intensity * vec3(1,1,1);
    vec3 ambient_light = ka * light_intensity * colour;
    vec3 diff_light    = kd * n_dot_w * light_intensity * colour;
    vec3 l1 = diff_light + spec_light;

    frag_colour = vec4(l1, 1.0);
}