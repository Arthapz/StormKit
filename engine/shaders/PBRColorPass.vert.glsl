#version 460 core
#extension GL_ARB_separate_shader_objects : enable
#pragma shader_stage(vertex)

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_texcoord;
layout(location = 3) in vec4 vertex_tangent;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec2 out_texcoord;
layout(location = 3) out vec4 out_tangent;

layout(set = 0, binding = 0, std140) uniform Camera {
    vec4 position;
    mat4 projection;
    mat4 view;
} camera;

layout(set = 1, binding = 0, std140) uniform Transform {
    mat4 model;
    mat4 inverted_model;
} transform;


void main() {
    vec4 model_space_position = transform.model * vec4(vertex_position, 1.f);
    //model_space_position.y = -model_space_position.y;

    out_position = model_space_position.xyz / model_space_position.w;
    out_normal   = normalize(transpose(inverse(mat3(transform.model))) * vertex_normal);
    out_texcoord = vertex_texcoord;
    out_tangent  = vertex_tangent;

    gl_Position = camera.projection * camera.view * vec4(out_position, 1.f);
}
