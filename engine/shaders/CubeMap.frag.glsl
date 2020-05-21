#version 460 core

#pragma shader_stage(fragment)

layout(location = 0) in vec3 in_texcoord;

layout(location = 0) out vec4 out_color;

layout(set = 1, binding = 0) uniform samplerCube cube_map;


void main() {
    out_color = texture(cube_map, in_texcoord);
}
