// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>

#ifdef STORM_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

#include <vector>

#include <storm/core/Math.hpp>

#include <storm/graphics/core/Vertex.hpp>
#include <storm/graphics/scenegraph/Fwd.hpp>
#include <storm/graphics/scenegraph/Node.hpp>

namespace storm::graphics {
	class STORM_PUBLIC StaticGeometryNode final : public Node {
	  public:
		struct NodeData {
			VertexArrayProxy vertex_array;
			IndexArrayProxy index_array;
		};
		using NodeDataCRef = std::reference_wrapper<const NodeData>;

		StaticGeometryNode(std::string name = "");
		~StaticGeometryNode() override;

		StaticGeometryNode(StaticGeometryNode &&);
		StaticGeometryNode &operator=(StaticGeometryNode &&);

		inline void setIndexArray(IndexArrayProxy proxy) noexcept {
			m_data.index_array = proxy;

			notify(NodeEvent::Updated, UpdatedPayload{*this});
		}

		inline const IndexArrayProxy &indexArray() const noexcept {
			return m_data.index_array;
		}

		inline void setVertexArray(VertexArrayProxy proxy) noexcept {
			m_data.vertex_array = std::move(proxy);

			notify(NodeEvent::Updated, UpdatedPayload{*this});
		}

		inline const VertexArrayProxy &vertexArray() const noexcept {
			return m_data.vertex_array;
		}

		inline const NodeData &data() const noexcept {
			return m_data;
		}

	  protected:
		void visit(SceneVisitorContext &context,
				   SceneRenderer &renderer) const override;

	  private:
		NodeData m_data;
	};
} // namespace storm::graphics
