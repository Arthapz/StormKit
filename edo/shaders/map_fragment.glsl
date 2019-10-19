#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 frag_uv;

layout(location = 0) out vec4 out_color;

layout(set = 2, binding = 0) uniform sampler2D sampled_texture;

layout(set = 3, binding = 0) uniform Tileset {
    vec2 tilemapsize;
    vec2 tilesize;
    vec2 mapsize;
} tileset_info;
layout(set = 3, binding = 1) uniform sampler2D map;

void main() {
    vec2 cell_position = vec2(
        floor(frag_uv.x * tileset_info.mapsize.x),
        floor(frag_uv.y * tileset_info.mapsize.y)
    );
    
    vec2 current_tile = texture(map, cell_position / tileset_info.mapsize).xy;
    
    vec2 tile_position = vec2(
        (ceil(current_tile.x * 255) * tileset_info.tilesize.x) / (tileset_info.tilemapsize.x * tileset_info.tilesize.x),
        (ceil(current_tile.y * 255) * tileset_info.tilesize.y) / (tileset_info.tilemapsize.y * tileset_info.tilesize.y)
    );
    
    vec2 uv_offset = vec2(
        ((frag_uv.x * tileset_info.mapsize.x) - cell_position.x) / tileset_info.tilemapsize.x,
        ((frag_uv.y * tileset_info.mapsize.y) - cell_position.y) / tileset_info.tilemapsize.y
    );

    vec4 color = texture(sampled_texture, tile_position + uv_offset);
    if(color.w < 1.f) discard;
    
    out_color = color;
    
    
}
