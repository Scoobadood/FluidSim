#version 410 core

layout(location=0) in vec3 pos;
layout(location=1) in vec2 tex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 project;

out vec2 frag_tex_coord;

void main() {
  mat4 model_view = view * model;
  mat3 mvt = mat3(model_view);

  gl_PointSize = 10.0;
  gl_Position = project * model_view * vec4(pos, 1.0);
  frag_tex_coord = tex;
}