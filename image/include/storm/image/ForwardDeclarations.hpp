// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Memory.hpp>
#include <storm/core/NamedType.hpp>

namespace storm::image {
	class Image;
	DECLARE_PTR_AND_REF(Image)

	using XOffset = core::NamedType<std::uint32_t, struct XOffsetTag>;
	using YOffset = core::NamedType<std::uint32_t, struct YOffsetTag>;

	namespace private_ {
		struct ImageData;
		DECLARE_PTR_AND_REF(ImageData)
	} // namespace private_
} // namespace storm::image
