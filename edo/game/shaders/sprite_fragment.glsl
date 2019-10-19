#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 frag_uv;

layout(location = 0) out vec4 out_color;

layout(set = 2, binding = 0) uniform sampler2D sampled_texture;

void main() {
    vec4 color = texture(sampled_texture, frag_uv);
    if(color.w < 1.f) discard;

    out_color = color;
}
