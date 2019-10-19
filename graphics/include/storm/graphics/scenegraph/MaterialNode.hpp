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

#include <storm/graphics/scenegraph/Fwd.hpp>
#include <storm/graphics/scenegraph/Node.hpp>
#include <storm/render/resource/Fwd.hpp>

namespace storm::graphics {
	class STORM_PUBLIC MaterialNode final : public Node {
	  public:
		MaterialNode(std::string name = "");
		~MaterialNode() override;

		MaterialNode(MaterialNode &&);
		MaterialNode &operator=(MaterialNode &&);

		inline void setMaterial(Material material) noexcept {
			m_data = std::move(material);

			notify(NodeEvent::Updated, UpdatedPayload{*this});
		}

		inline const Material &material() const noexcept {
			return m_data;
		}

	  protected:
		void visit(SceneVisitorContext &context,
				   SceneRenderer &renderer) const override;

	  private:
		Material m_data;
	};
} // namespace storm::graphics
