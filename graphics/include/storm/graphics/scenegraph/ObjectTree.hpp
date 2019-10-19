// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <variant>

#include <storm/core/Memory.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Tree.hpp>
#include <storm/core/TreeNode.hpp>

#include <storm/core/Math.hpp>
#include <storm/render/pipeline/GraphicsPipelineState.hpp>

#include <storm/graphics/core/Material.hpp>
#include <storm/graphics/scenegraph/Fwd.hpp>

namespace storm::graphics {

	class ObjectTreeNode : public core::TreeNode, public core::NonCopyable {
	  public:
		struct AccumulatedData {
			core::Matrix transform = core::Matrix{1.f};

			render::GraphicsPipelineState state;
			Material material;
		};

		static constexpr auto TRANSFORM_DIRTY_BITS	= DirtyBitType{0b1};
		static constexpr auto VIEW_DIRTY_BITS		  = DirtyBitType{0b10};
		static constexpr auto PROJECTION_DIRTY_BITS   = DirtyBitType{0b100};
		static constexpr auto CHILD_ADDED_DIRTY_BITS  = DirtyBitType{0b1000};
		static constexpr auto CHILD_REMOVE_DIRTY_BITS = DirtyBitType{0b10000};
		static constexpr auto DATA_UPDATE_DIRTY_BITS  = DirtyBitType{0b100000};

		ObjectTreeNode()  = default;
		~ObjectTreeNode() = default;

		ObjectTreeNode(ObjectTreeNode &&) = default;
		ObjectTreeNode &operator=(ObjectTreeNode &&) = default;

		inline void setAccumulatedData(AccumulatedData &&data) {
			m_data = std::move(data);
		}
		inline const AccumulatedData &data() const noexcept {
			return m_data;
		}

	  private:
		AccumulatedData m_data;
	};

	using ObjectTree = core::Tree<ObjectTreeNode>;
	DECLARE_PTR_AND_REF(ObjectTree)
} // namespace storm::graphics
