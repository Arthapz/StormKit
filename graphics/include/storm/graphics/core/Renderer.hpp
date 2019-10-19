#pragma once

#include <unordered_set>

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/RingBuffer.hpp>

#include <storm/core/Math.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Surface.hpp>
#include <storm/render/pipeline/Fwd.hpp>
#include <storm/render/pipeline/GraphicsPipelineState.hpp>
#include <storm/render/resource/Fwd.hpp>
#include <storm/render/sync/Fwd.hpp>

#include <storm/graphics/core/Fwd.hpp>
#include <storm/graphics/core/Material.hpp>
#include <storm/graphics/framegraph/FrameGraphBuilder.hpp>
#include <storm/graphics/framegraph/Fwd.hpp>

namespace storm::graphics {
	struct MaterialData {
		Material material;
		render::DescriptorSetOwnedPtr set;
	};
} // namespace storm::graphics

HASH_FUNC(storm::graphics::MaterialData)

namespace storm::graphics {
	class STORM_PUBLIC Renderer : public core::NonCopyable,
								  public core::NonMovable {
	  public:
		Renderer(const render::Device &device, render::Surface &surface);
		~Renderer();

		inline void setProjectionMatrix(core::Matrix matrix) noexcept {
			// auto lock = std::unique_lock{m_mutex};

			if (matrix == m_camera_matrices.projection)
				return;

			m_camera_matrices.projection  = std::move(matrix);
			m_camera_matrices_need_update = true;
		}

		inline void setViewMatrix(core::Matrix matrix) noexcept {
			// auto lock = std::unique_lock{m_mutex};

			if (matrix == m_camera_matrices.view)
				return;

			m_camera_matrices.view		  = std::move(matrix);
			m_camera_matrices_need_update = true;
		}

		void present();

		void drawMeshes(render::CommandBuffer &cmb,
						const render::RenderPass &render_pass);
		void drawMeshesByMaterial(
			const render::DescriptorSet &camera_set,
			const render::GraphicsPipelineState &pipeline_state,
			const MaterialData &material_data, render::CommandBuffer &cmb,
			const render::RenderPass &render_pass);
		void addMesh(const render::GraphicsPipelineState &state,
					 const Mesh &mesh);
		void addMesh(const render::GraphicsPipelineState &state,
					 const Material &material, const Mesh &mesh);

		[[nodiscard]] inline FrameGraphBuilder &frameBuilder() noexcept {
			return m_builder;
		}

		[[nodiscard]] inline std::vector<
			render::DescriptorSetLayoutConstObserverPtr>
			defaultLayout() const {
			return {
				core::makeConstObserver(m_camera_descriptor_set_layout),
				core::makeConstObserver(m_per_mesh_descriptor_set_layout),
				core::makeConstObserver(m_per_material_descriptor_set_layout)};
		}

	  private:
		static constexpr auto MAX_CAMERA_SETS = std::uint32_t{1};
		static constexpr auto MAX_MATERIAL_SETS =
			gsl::narrow_cast<std::uint32_t>(
				std::numeric_limits<std::uint16_t>::max());

		void updateCamera(std::uint32_t frame_index);

		render::DeviceConstObserverPtr m_device;
		render::SurfaceObserverPtr m_surface;
		render::QueueConstObserverPtr m_graphics_queue;
		render::QueueConstObserverPtr m_transfert_queue;

		render::PipelineCacheOwnedPtr m_pipeline_cache;

		struct CameraMatrices {
			core::Matrix projection;
			core::Matrix view;
		} m_camera_matrices;
		bool m_camera_matrices_need_update = false;
		render::HardwareBufferOwnedPtr m_per_camera_data_buffer;
		std::vector<render::HardwareBufferOwnedPtr> m_per_camera_datas_staging;

		std::vector<render::CommandBufferOwnedPtr> m_matrices_transfert_cmds;
		std::vector<render::SemaphoreOwnedPtr> m_matrices_transfert_finished;

		render::DescriptorSetLayoutOwnedPtr m_camera_descriptor_set_layout;
		render::DescriptorPoolOwnedPtr m_camera_descriptor_pool;
		render::DescriptorSetOwnedPtr m_per_camera_set;

		render::Surface::Frame m_frame;
		FrameGraphBuilder m_builder;
		core::RingBuffer<FrameGraph> m_framegraphs;
		FrameGraphObserverPtr m_current_framegraph;

		render::DescriptorSetLayoutOwnedPtr m_per_mesh_descriptor_set_layout;

		render::DescriptorSetLayoutOwnedPtr
			m_per_material_descriptor_set_layout;
		render::DescriptorPoolOwnedPtr m_material_descriptor_pool;

		std::unordered_map<render::GraphicsPipelineState,
						   std::unordered_set<MaterialData>>
			m_materials;
		std::unordered_map<Material, MeshCacheOwnedPtr> m_meshes;
		std::vector<render::CommandBufferOwnedPtr> m_blit_cmds;

		std::vector<render::SemaphoreOwnedPtr> m_render_finished;
		render::ShaderOwnedPtr m_quad_vert_shader;
        render::ShaderOwnedPtr m_quad_frag_shader;
        render::DescriptorSetLayoutOwnedPtr m_quad_descriptor_set_layout;
        render::DescriptorPoolOwnedPtr m_quad_descriptor_pool;
        std::vector<render::DescriptorSetOwnedPtr> m_quad_descriptor_sets;
        render::HardwareBufferOwnedPtr m_quad_vertices;
        render::RenderPassOwnedPtr m_quad_render_pass;
		render::GraphicsPipelineOwnedPtr m_quad_pipeline;
		std::vector<render::FramebufferOwnedPtr> m_quad_framebuffers;
        std::vector<render::CommandBufferOwnedPtr> m_quad_cmds;
        render::SamplerOwnedPtr m_quad_sampler;
	};
} // namespace storm::graphics
