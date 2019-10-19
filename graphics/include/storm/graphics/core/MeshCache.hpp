#pragma once

#include <unordered_map>

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/render/core/Device.hpp>

#include <storm/graphics/core/Fwd.hpp>

namespace storm::graphics {
    class STORM_PUBLIC MeshCache : public core::NonCopyable {
      public:
        MeshCache(std::uint32_t image_count, const render::Device &device,
                  const render::DescriptorSetLayout &layout);
        ~MeshCache();

        MeshCache(MeshCache &&);
        MeshCache &operator=(MeshCache &&);

        void addMesh(const Mesh &mesh);
        std::vector<render::SemaphoreConstObserverPtr>
            drawMeshes(render::CommandBuffer &cmb,
                       const render::GraphicsPipeline &pipeline,
                       const render::DescriptorSet &camera_set,
                       const render::DescriptorSet &material_set,
                       std::uint32_t frame_id) const;

      private:
        struct MeshData {
            std::vector<render::SemaphoreOwnedPtr> model_transfert_semaphores;
            std::vector<render::CommandBufferOwnedPtr> model_transfert_cmbs;
            std::vector<render::HardwareBufferOwnedPtr> model_staging_buffers;
            render::HardwareBufferOwnedPtr model_buffer;

            render::DescriptorSetOwnedPtr model_set;
        };

        void updateCameraMeshData(const Mesh &mesh, const MeshData &mesh_data,
                                  std::uint32_t frame_id) const;

        static constexpr auto MAX_MESH_SETS = gsl::narrow_cast<std::uint32_t>(
            std::numeric_limits<std::uint16_t>::max());

        std::uint32_t m_image_count;

        render::DeviceConstObserverPtr m_device;
        render::QueueConstObserverPtr m_graphics_queue;
        render::DescriptorSetLayoutConstObserverPtr m_descriptor_set_layout;
        render::DescriptorPoolOwnedPtr m_descriptor_pool;

        std::unordered_map<MeshConstObserverPtr, MeshData> m_meshes;
    };
} // namespace storm::graphics
