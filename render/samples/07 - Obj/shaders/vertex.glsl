#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

layout(location = 0) out vec2 tex_coords;

layout(binding = 0) uniform Matrices {
    mat4 projection;
    mat4 view;
    mat4 model;
} matrices;

void main() {
    gl_Position = matrices.projection * matrices.view * matrices.model * vec4(position, 1.f);
    gl_Position.y = -gl_Position.y;
    gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0;
    tex_coords = uv;
}
