#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 tex_coords;

layout(location = 0) out vec4 out_color;

layout(binding = 1) uniform sampler2D diffuse;

void main() {
    out_color = texture(diffuse, tex_coords);
}
