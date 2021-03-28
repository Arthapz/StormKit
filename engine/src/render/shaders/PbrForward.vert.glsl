#version 460 core
#extension GL_ARB_separate_shader_objects : enable
#pragma shader_stage(vertex)

#define MAX_TRANSFORM_COUNT 255

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(location = 3) in int transform_id;
layout(location = 4) in int material_id;
layout(location = 5) in int skinning_id;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec2 out_uv;

layout(set = 0, binding = 0, std140) uniform SceneData {
    vec4 position;
    mat4 projection;
    mat4 view;
} scene_data;

struct Transform {
    mat4 model;
    mat4 inv_model;
};

layout(set = 1, binding = 0, std140) readonly buffer Transforms {
    Transform data[];
} transforms;
/*layout(set = 1, binding = 0, std140) readonly buffer Materials {
    Material data[];
} materials;
layout(set = 1, binding = 0, std140) readonly buffer Skinnings {
    Skinning data[];
} skinnings;*/

out gl_PerVertex  {
    vec4 gl_Position;
};

void main() {
    const mat4 model = transforms.data[transform_id].model;
    const mat4 inv_model = transforms.data[transform_id].inv_model;

    const vec4 model_space_position = model * vec4(position, 1.f);

    out_position = model_space_position.xyz / model_space_position.w;
    out_normal = normalize(transpose(mat3(inv_model)) * normal);
    out_uv = uv;

    gl_Position = scene_data.projection * scene_data.view * model_space_position;
}
