#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 frag_uv;

layout(binding = 0) uniform sampler2D backbuffer;

layout(location = 0) out vec4 out_color;

void main() {
    out_color = texture(backbuffer, frag_uv);
}
