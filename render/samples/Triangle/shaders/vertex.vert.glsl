#version 460 core
#extension GL_ARB_separate_shader_objects : enable
#pragma shader_stage(vertex)

layout(location = 0) out vec3 frag_color;

vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);


vec2 positions[3] = vec2[](
    vec2(-0.5, 0.5),
    vec2(0.5, 0.5),
    vec2(0.0, -0.5)
);

void main() {
    vec4 p = vec4(positions[gl_VertexIndex], 0.f, 1.f);
    p.y = -p.y;

    gl_Position = p;

    frag_color = colors[gl_VertexIndex];
}
