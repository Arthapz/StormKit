#version 460 core
#extension GL_ARB_separate_shader_objects : enable
#pragma shader_stage(vertex)

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 frag_color;

layout(set = 0, binding = 0, std140) uniform Camera {
    mat4 projection;
    mat4 view;
} camera;

layout(set = 1, binding = 0, std140) uniform MeshData {
    mat4 model;
} mesh_data;

void main() {
    vec4 p = vec4(position, 1.f);
    p.y = -p.y;

    gl_Position = camera.projection * camera.view * mesh_data.model * p;

    frag_color = color;
}
