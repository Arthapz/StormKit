// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Memory.hpp>

#include <storm/render/resource/Fwd.hpp>

namespace storm::graphics {
	class SceneGraph;
	DECLARE_PTR_AND_REF(SceneGraph);

	class Node;
	DECLARE_PTR_AND_REF(Node);

	class SceneNode;
	DECLARE_PTR_AND_REF(SceneNode);

	class CameraNode;
	DECLARE_PTR_AND_REF(CameraNode);

	class PipelineNode;
	DECLARE_PTR_AND_REF(PipelineNode);

	class TransformNode;
	DECLARE_PTR_AND_REF(TransformNode);

	class MaterialNode;
	DECLARE_PTR_AND_REF(MaterialNode);

	class StaticGeometryNode;
	DECLARE_PTR_AND_REF(StaticGeometryNode);

	class SceneRenderer;
	DECLARE_PTR_AND_REF(SceneRenderer);

	class SceneGraphObserver;
	DECLARE_PTR_AND_REF(SceneGraphObserver);

	class DrawableManager;
	DECLARE_PTR_AND_REF(DrawableManager);

	struct SceneVisitorContext;
} // namespace storm::graphics
