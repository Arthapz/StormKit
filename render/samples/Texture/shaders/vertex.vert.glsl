#version 460 core
#extension GL_ARB_separate_shader_objects : enable
#pragma shader_stage(vertex)

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv;

layout(location = 0) out vec2 frag_uv;

layout(set = 0, binding = 0) uniform Camera {
    mat4 projection;
    mat4 view;
} camera;

layout(set = 1, binding = 0) uniform MeshData {
    mat4 model;
} mesh_data;

void main() {
    gl_Position = camera.projection * camera.view * mesh_data.model * vec4(position, 0.f, 1.f);
    gl_Position.y = -gl_Position.y;
    frag_uv = uv;
}
