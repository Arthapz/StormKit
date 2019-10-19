#version 460 core
#extension GL_ARB_separate_shader_objects : enable
#pragma shader_stage(vertex)

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(location = 0) out vec3 frag_position;
layout(location = 1) out vec3 frag_normal;
layout(location = 2) out vec2 frag_uv;

layout(set = 0, binding = 0, std140) uniform Camera {
    mat4 projection;
    mat4 view;
} camera;

layout(set = 1, binding = 0, std140) uniform Transform {
    mat4 model;
    mat4 inv_model;
} transform;

void main() {
    vec4 model_space_position = transform.model * vec4(position, 1.0);

    gl_Position = camera.projection * camera.view * model_space_position;

    frag_position = model_space_position.xyz;
    frag_normal = mat3(transpose(transform.inv_model)) * normal;
    frag_uv = uv;

    gl_Position.y = -gl_Position.y;
}
