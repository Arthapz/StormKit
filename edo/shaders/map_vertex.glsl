#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv;

layout(location = 0) out vec2 frag_uv;

layout(set = 0, binding = 0) uniform Camera {
    mat4 projection;
    mat4 view;
} camera_data;

layout(set = 1, binding = 0) uniform Model {
    mat4 model;
} model_data;

void main() {
    gl_Position = camera_data.projection * camera_data.view * model_data.model * vec4(position, 0.0, 1.0);
    
    frag_uv = uv;
}
