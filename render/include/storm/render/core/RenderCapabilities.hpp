// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <cstdint>

#include <storm/render/core/Enums.hpp>

namespace storm::render {
    struct RenderCapabilities {
        struct {
            bool robust_buffer_access;
            bool full_draw_index_uint32;
            bool image_cube_array;
            bool independent_blend;
            bool geometry_shader;
            bool tessellation_shader;
            bool sampler_rate_shading;
            bool dual_src_blend;
            bool logic_op;
            bool multi_draw_indirect;
            bool draw_indirect_first_instance;
            bool depth_clamp;
            bool depth_bias_clamp;
            bool fill_Mode_non_solid;
            bool depth_bounds;
            bool wide_lines;
            bool large_points;
            bool alpha_to_one;
            bool multi_viewport;
            bool sampler_anisotropy;
            bool texture_compression_etc2;
            bool texture_compression_astc_ldr;
            bool texture_compression_bc;
            bool occlusion_query_precise;
            bool pipeline_statistics_query;
            bool vertex_pipeline_stores_and_atomics;
            bool fragment_stores_and_atomics;
            bool shader_tessellation_and_geometry_point_size;
            bool shader_image_gather_extended;
            bool shader_storage_image_extended_formats;
            bool shader_storage_image_multisample;
            bool shader_storage_image_read_without_format;
            bool shader_storage_image_write_without_format;
            bool shader_uniform_buffer_array_dynamic_indexing;
            bool shader_sampled_image_array_dynamic_indexing;
            bool shader_storage_buffer_array_dynamic_indexing;
            bool shader_storage_image_array_dynamic_indexing;
            bool shader_clip_distance;
            bool shader_cull_distance;
            bool shader_float_64;
            bool shader_int_64;
            bool shader_int_16;
            bool shader_resource_residency;
            bool shader_resource_min_lod;
            bool sparse_binding;
            bool sparse_residency_buffer;
            bool sparse_residency_image_2D;
            bool sparse_residency_image_3D;
            bool sparse_residency_2_samples;
            bool sparse_residency_4_samples;
            bool sparse_residency_6_samples;
            bool sparse_residency_8_samples;
            bool sparse_residency_16_samples;
            bool sparse_residency_aliased;
            bool variable_multisample_rate;
            bool inherited_queries;
        } features;

        struct {
            core::UInt32 max_image_dimension_1D;
            core::UInt32 max_image_dimension_2D;
            core::UInt32 max_image_dimension_3D;
            core::UInt32 max_image_dimension_cube;
            core::UInt32 max_image_array_layers;
            core::UInt32 max_texel_buffer_elements;
            core::UInt32 max_uniform_buffer_range;
            std::optional<core::UInt32> max_storage_buffer_range;
            core::UInt32 max_push_constants_size;
            std::optional<core::UInt32> max_memory_allocation_count;
            std::optional<core::UInt32> max_sampler_allocation_count;
            std::optional<core::UInt64> buffer_image_granularity;
            std::optional<core::UInt64> sparse_address_space_size;
            std::optional<core::UInt32> max_bound_descriptor_sets;
            core::UInt32 max_per_stage_descriptor_samplers;
            core::UInt32 max_per_stage_descriptor_uniform_buffers;
            core::UInt32 max_per_stage_descriptor_storage_buffers;
            core::UInt32 max_per_stage_descriptor_sampled_images;
            core::UInt32 max_per_stage_descriptor_storage_images;
            std::optional<core::UInt32> max_per_stage_descriptor_input_attachments;
            std::optional<core::UInt32> max_per_stage_resources;
            core::UInt32 max_descriptor_set_samplers;
            core::UInt32 max_descriptor_set_uniform_buffers;
            core::UInt32 max_descriptor_set_uniform_buffers_dynamic;
            core::UInt32 max_descriptor_set_storage_buffers;
            core::UInt32 max_descriptor_set_storage_buffers_dynamic;
            core::UInt32 max_descriptor_set_sampled_images;
            core::UInt32 max_descriptor_set_storage_images;
            std::optional<core::UInt32> max_descriptor_set_input_attachments;
            core::UInt32 max_vertex_input_attributes;
            core::UInt32 max_vertex_input_bindings;
            core::UInt32 max_vertex_input_attribute_offset;
            std::optional<core::UInt32> max_vertex_input_binding_stride;
            core::UInt32 max_vertex_output_components;
            core::UInt32 max_tessellation_generation_level;
            core::UInt32 max_tessellation_patch_size;
            core::UInt32 max_tessellation_control_per_vertex_input_components;
            core::UInt32 max_tessellation_control_per_vertex_output_components;
            core::UInt32 max_tessellation_control_per_patch_output_components;
            core::UInt32 max_tessellation_control_total_output_components;
            core::UInt32 max_tessellation_evaluation_input_components;
            core::UInt32 max_tessellation_evaluation_output_components;
            core::UInt32 max_geometry_shader_invocations;
            core::UInt32 max_geometry_input_components;
            core::UInt32 max_geometry_output_components;
            core::UInt32 max_geometry_output_vertices;
            core::UInt32 max_geometry_total_output_components;
            core::UInt32 max_fragment_input_components;
            core::UInt32 max_fragment_output_attachments;
            core::UInt32 max_fragment_dual_src_attachments;
            core::UInt32 max_fragment_combined_output_resources;
            core::UInt32 max_compute_shared_memory_size;
            std::array<core::UInt32, 3> max_compute_work_group_count;
            core::UInt32 max_compute_work_group_invocations;
            std::array<core::UInt32, 3> max_compute_work_group_size;
            std::optional<core::UInt32> sub_pixel_precision_bits;
            std::optional<core::UInt32> sub_texel_precision_bits;
            std::optional<core::UInt32> mipmap_precision_bits;
            core::UInt32 max_draw_indexed_index_value;
            std::optional<core::UInt32> max_draw_indirect_count;
            float max_sampler_lod_bias;
            float max_sampler_anisotropy;
            core::UInt32 max_viewports;
            std::array<core::UInt32, 2> max_viewport_dimensions;
            std::array<float, 2> viewport_bounds_range;
            std::optional<core::UInt32> viewport_sub_pixel_bits;
            std::optional<core::ArraySize> min_memory_map_alignment;
            std::optional<core::UInt64> min_texel_buffer_offset_alignment;
            core::UInt64 min_uniform_buffer_offset_alignment;
            core::UInt64 min_storage_buffer_offset_alignment;
            core::Int32 min_texel_offset;
            core::UInt32 max_texel_offset;
            core::Int32 min_texel_gather_offset;
            core::UInt32 max_texel_gather_offset;
            float min_interpolation_offset;
            float max_interpolation_offset;
            std::optional<core::UInt32> sub_pixel_interpolation_offset_bits;
            core::UInt32 max_framebuffer_width;
            core::UInt32 max_framebuffer_height;
            core::UInt32 max_framebuffer_layers;
            SampleCountFlag framebuffer_color_sample_counts;
            SampleCountFlag framebuffer_depth_sample_counts;
            SampleCountFlag framebuffer_stencil_sample_counts;
            SampleCountFlag framebuffer_no_attachments_sample_counts;
            core::UInt32 max_color_attachments;
            SampleCountFlag sampled_image_color_sample_counts;
            SampleCountFlag sampled_image_integer_sample_counts;
            SampleCountFlag sampled_image_depth_sample_counts;
            SampleCountFlag sampled_image_stencil_sample_counts;
            SampleCountFlag storage_image_sample_counts;
            core::UInt32 max_sample_mask_words;
            bool timestamp_compute_and_engine;
            float timestamp_period;
            core::UInt32 max_clip_distances;
            core::UInt32 max_cull_distances;
            core::UInt32 max_combined_clip_and_cull_distances;
            core::UInt32 discrete_queue_priorities;
            std::array<float, 2> point_size_range;
            std::array<float, 2> line_width_range;
            float point_size_granularity;
            float line_width_granularity;
            bool strict_lines;
            bool standard_sample_locations;
            std::optional<core::UInt64> optimal_buffer_copy_offset_alignment;
            std::optional<core::UInt64> optimal_buffer_copy_row_pitch_alignment;
            core::UInt64 non_coherent_atom_size;
        } limits;
    };
} // namespace storm::render
