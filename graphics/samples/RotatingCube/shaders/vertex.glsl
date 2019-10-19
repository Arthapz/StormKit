#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 frag_color;

layout(set = 0, binding = 0) uniform Camera {
    mat4 projection;
    mat4 view;
} camera_data;

layout(set = 1, binding = 0) uniform Model {
    mat4 model;
} model_data;

void main() {
    gl_Position = camera_data.projection * camera_data.view * model_data.model * vec4(position, 1.0);
    frag_color = color;
}
