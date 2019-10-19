#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv;

layout(location = 0) out vec2 frag_uv;

void main() {
    gl_Position = vec4(position, 0.f, 1.f);

    frag_uv = uv;
}
