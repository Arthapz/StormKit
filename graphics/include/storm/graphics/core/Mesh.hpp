#pragma once

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/RingBuffer.hpp>

#include <storm/render/core/Device.hpp>
#include <storm/render/core/Surface.hpp>
#include <storm/core/Math.hpp>

#include <storm/graphics/core/Fwd.hpp>
#include <storm/graphics/core/Vertex.hpp>

namespace storm::graphics {
	class STORM_PUBLIC Mesh : public core::NonCopyable {
      public:
        Mesh(const render::Device &device);
		~Mesh();

		Mesh(Mesh &&);
		Mesh &operator=(Mesh &&);

        void draw(render::CommandBuffer &cmb) const;

		void setVertexData(VertexArrayProxy vertex_data,
                           std::uint32_t vertex_count);
		void setIndexData(IndexArrayProxy index_data,
                          std::uint32_t index_count);
        inline void setModelMatrix(core::Matrix matrix) {
            if (m_model_data.matrix == matrix)
                return;

            m_model_data.matrix		 = std::move(matrix);
            m_model_data_need_update = true;
        }

        [[nodiscard]] inline const core::Matrix &modelMatrix() const
            noexcept {
            return m_model_data.matrix;
        }

        [[nodiscard]] inline std::uint32_t vertexCount() const noexcept {
			return m_vertex_count;
		}
        [[nodiscard]] inline std::uint32_t indexCount() const noexcept {
			return m_index_count;
		}

        inline void addCustomSet(render::DescriptorSetConstObserverPtr set) {
            m_custom_sets.emplace_back(set);
        }

        inline gsl::span<const render::DescriptorSetConstObserverPtr>
            customSets() const noexcept {
            return m_custom_sets;
        }

	  private:
        struct ModelData {
            core::Matrix matrix = core::Matrix{1.f};
        };

        render::DeviceConstObserverPtr m_device;
        render::DescriptorPoolObserverPtr m_descriptor_pool;

		render::HardwareBufferOwnedPtr m_vertex_buffer;
		std::size_t m_vertex_buffer_size = 0;
		std::uint32_t m_vertex_count	 = 0;

		render::HardwareBufferOwnedPtr m_index_buffer;
		std::size_t m_index_buffer_size = 0;
		std::uint32_t m_index_count		= 0;
		bool m_use_large_indices		= false;

        ModelData m_model_data;
        mutable bool m_model_data_need_update = true; // TODO viré le mutable

        std::vector<render::DescriptorSetConstObserverPtr> m_custom_sets;

        friend class MeshCache;
	};
} // namespace storm::graphics
