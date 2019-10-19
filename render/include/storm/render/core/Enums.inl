// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "Enums.hpp"

#define AS(X, Y)                                                               \
	case X:                                                                    \
		return Y;

#define FLAG_COMPARE(X, Y, Z)                                                  \
	if ((X & Y) == Y) {                                                        \
		Z;                                                                     \
	}

#define FROM(X, Y)                                                             \
	case Y:                                                                    \
		return X;

namespace storm::render {
	inline constexpr render::PhysicalDeviceType
		fromVK(VkPhysicalDeviceType type) noexcept {
		switch (type) {
			AS(VK_PHYSICAL_DEVICE_TYPE_CPU, PhysicalDeviceType::CPU)
			AS(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
			   PhysicalDeviceType::Discrete_GPU)
			AS(VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU,
			   PhysicalDeviceType::Virtual_GPU)
			AS(VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU,
			   PhysicalDeviceType::Integrated_GPU)
		default:
			break;
		}

		return {};
	}

	inline constexpr render::QueueFlag fromVK(VkQueueFlags vk_flag) noexcept {
		auto flag = render::QueueFlag{};

		FLAG_COMPARE(vk_flag, VK_QUEUE_GRAPHICS_BIT,
					 flag |= render::QueueFlag::Graphics)
		FLAG_COMPARE(vk_flag, VK_QUEUE_COMPUTE_BIT,
					 flag |= render::QueueFlag::Compute)
		FLAG_COMPARE(vk_flag, VK_QUEUE_TRANSFER_BIT,
					 flag |= render::QueueFlag::Transfert)
		FLAG_COMPARE(vk_flag, VK_QUEUE_SPARSE_BINDING_BIT,
					 flag |= render::QueueFlag::Sparse_Binding)

		return flag;
	}

	inline constexpr VkShaderStageFlags toVK(render::ShaderType mode) noexcept {
		auto flag = VkShaderStageFlags{};

		FLAG_COMPARE(mode, render::ShaderType::Vertex,
					 flag |= VK_SHADER_STAGE_VERTEX_BIT)
		FLAG_COMPARE(mode, render::ShaderType::Fragment,
					 flag |= VK_SHADER_STAGE_FRAGMENT_BIT)
		FLAG_COMPARE(mode, render::ShaderType::Geometry,
					 flag |= VK_SHADER_STAGE_GEOMETRY_BIT)
		FLAG_COMPARE(mode, render::ShaderType::Compute,
					 flag |= VK_SHADER_STAGE_COMPUTE_BIT)

		return flag;
	}

	inline constexpr VkPrimitiveTopology
		toVK(render::PrimitiveTopology type) noexcept {
		switch (type) {
			AS(render::PrimitiveTopology::Point_List,
			   VK_PRIMITIVE_TOPOLOGY_POINT_LIST)
			AS(render::PrimitiveTopology::Line_List,
			   VK_PRIMITIVE_TOPOLOGY_LINE_LIST)
			AS(render::PrimitiveTopology::Line_Strip,
			   VK_PRIMITIVE_TOPOLOGY_LINE_STRIP)
			AS(render::PrimitiveTopology::Triangle_List,
			   VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
			AS(render::PrimitiveTopology::Triangle_Strip,
			   VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP)
			AS(render::PrimitiveTopology::Triangle_Fan,
			   VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN)
		}

		return {};
	}

	inline constexpr VkPolygonMode toVK(render::PolygonMode mode) noexcept {
		switch (mode) {
			AS(render::PolygonMode::Fill, VK_POLYGON_MODE_FILL)
			AS(render::PolygonMode::Line, VK_POLYGON_MODE_LINE)
			AS(render::PolygonMode::Point, VK_POLYGON_MODE_POINT)
		}

		return {};
	}

	inline constexpr VkCullModeFlags toVK(render::CullMode mode) noexcept {
		auto flag = VkCullModeFlags{VK_CULL_MODE_NONE};

		FLAG_COMPARE(mode, render::CullMode::Front,
					 flag |= VK_CULL_MODE_FRONT_BIT)
		FLAG_COMPARE(mode, render::CullMode::Back,
					 flag |= VK_CULL_MODE_BACK_BIT)

		return flag;
	}

	inline constexpr VkFrontFace toVK(render::FrontFace face) noexcept {
		switch (face) {
			AS(render::FrontFace::Clockwise, VK_FRONT_FACE_CLOCKWISE)
			AS(render::FrontFace::Counter_Clockwise,
			   VK_FRONT_FACE_COUNTER_CLOCKWISE)
		}

		return {};
	}

	inline constexpr VkSampleCountFlagBits
		toVK(render::SampleCountFlag flags) noexcept {
		auto flag = 0;

		FLAG_COMPARE(flags, render::SampleCountFlag::C1_BIT,
					 flag |= VK_SAMPLE_COUNT_1_BIT)
		FLAG_COMPARE(flags, render::SampleCountFlag::C2_BIT,
					 flag |= VK_SAMPLE_COUNT_2_BIT)
		FLAG_COMPARE(flags, render::SampleCountFlag::C4_BIT,
					 flag |= VK_SAMPLE_COUNT_4_BIT)
		FLAG_COMPARE(flags, render::SampleCountFlag::C8_BIT,
					 flag |= VK_SAMPLE_COUNT_8_BIT)
		FLAG_COMPARE(flags, render::SampleCountFlag::C16_BIT,
					 flag |= VK_SAMPLE_COUNT_16_BIT)
		FLAG_COMPARE(flags, render::SampleCountFlag::C32_BIT,
					 flag |= VK_SAMPLE_COUNT_32_BIT)
		FLAG_COMPARE(flags, render::SampleCountFlag::C64_BIT,
					 flag |= VK_SAMPLE_COUNT_64_BIT)

		return static_cast<VkSampleCountFlagBits>(flag);
    }

    using VkSampleCountFlagsType =
        core::NamedType<VkSampleCountFlags, struct VkSampleCountFlagsTag>;
    inline constexpr render::SampleCountFlag
        fromVK(VkSampleCountFlagsType proxy) noexcept {
        auto flags = proxy.get();
        auto flag  = render::SampleCountFlag{};

        FLAG_COMPARE(flags, VK_SAMPLE_COUNT_1_BIT,
                     flag |= render::SampleCountFlag::C1_BIT)
        FLAG_COMPARE(flags, VK_SAMPLE_COUNT_2_BIT,
                     flag |= render::SampleCountFlag::C2_BIT)
        FLAG_COMPARE(flags, VK_SAMPLE_COUNT_4_BIT,
                     flag |= render::SampleCountFlag::C4_BIT)
        FLAG_COMPARE(flags, VK_SAMPLE_COUNT_8_BIT,
                     flag |= render::SampleCountFlag::C8_BIT)
        FLAG_COMPARE(flags, VK_SAMPLE_COUNT_16_BIT,
                     flag |= render::SampleCountFlag::C16_BIT)
        FLAG_COMPARE(flags, VK_SAMPLE_COUNT_32_BIT,
                     flag |= render::SampleCountFlag::C32_BIT)
        FLAG_COMPARE(flags, VK_SAMPLE_COUNT_64_BIT,
                     flag |= render::SampleCountFlag::C64_BIT)

        return flag;
    }

	inline constexpr VkColorComponentFlags toVK(render::ColorComponent mode) {
		auto flag = VkColorComponentFlags{};

		FLAG_COMPARE(mode, render::ColorComponent::R,
					 flag |= VK_COLOR_COMPONENT_R_BIT)
		FLAG_COMPARE(mode, render::ColorComponent::G,
					 flag |= VK_COLOR_COMPONENT_G_BIT)
		FLAG_COMPARE(mode, render::ColorComponent::B,
					 flag |= VK_COLOR_COMPONENT_B_BIT)
		FLAG_COMPARE(mode, render::ColorComponent::A,
					 flag |= VK_COLOR_COMPONENT_A_BIT)

		return flag;
	}

	inline constexpr VkBlendFactor toVK(render::BlendFactor factor) noexcept {
		switch (factor) {
			AS(render::BlendFactor::One, VK_BLEND_FACTOR_ONE)
			AS(render::BlendFactor::Zero, VK_BLEND_FACTOR_ZERO)
			AS(render::BlendFactor::Src_Color, VK_BLEND_FACTOR_SRC_COLOR)
			AS(render::BlendFactor::One_Minus_Src_Color,
			   VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR)
			AS(render::BlendFactor::Dst_Color, VK_BLEND_FACTOR_DST_COLOR)
			AS(render::BlendFactor::One_Minus_Dst_Color,
			   VK_BLEND_FACTOR_DST_COLOR)
			AS(render::BlendFactor::Src_Alpha, VK_BLEND_FACTOR_SRC_ALPHA)
			AS(render::BlendFactor::One_Minus_Src_Alpha,
			   VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA)
			AS(render::BlendFactor::Dst_Alpha, VK_BLEND_FACTOR_DST_ALPHA)
			AS(render::BlendFactor::One_Minus_Dst_Alpha,
			   VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA)
			AS(render::BlendFactor::Constant_Color,
			   VK_BLEND_FACTOR_CONSTANT_COLOR)
			AS(render::BlendFactor::One_Minus_Constant_Color,
			   VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR)
			AS(render::BlendFactor::Constant_Alpha,
			   VK_BLEND_FACTOR_CONSTANT_ALPHA)
			AS(render::BlendFactor::One_Minus_Constant_Alpha,
			   VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA)
			AS(render::BlendFactor::Src_Alpha_Saturate,
			   VK_BLEND_FACTOR_SRC_ALPHA_SATURATE)
			AS(render::BlendFactor::Src1_Color, VK_BLEND_FACTOR_SRC1_COLOR)
			AS(render::BlendFactor::One_Minus_Src1_Color,
			   VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR)
			AS(render::BlendFactor::Src1_Alpha, VK_BLEND_FACTOR_SRC1_ALPHA)
			AS(render::BlendFactor::One_Minus_Src1_Alpha,
			   VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA)
		}

		return {};
	}

	inline constexpr VkBlendOp toVK(render::BlendOperation operation) noexcept {
		switch (operation) {
			AS(render::BlendOperation::Add, VK_BLEND_OP_ADD)
			AS(render::BlendOperation::Substract, VK_BLEND_OP_SUBTRACT)
			AS(render::BlendOperation::Reverse_Substract,
			   VK_BLEND_OP_REVERSE_SUBTRACT)
			AS(render::BlendOperation::Min, VK_BLEND_OP_MIN)
			AS(render::BlendOperation::Max, VK_BLEND_OP_MAX)
		}

		return {};
	}

	inline constexpr VkLogicOp toVK(render::LogicOperation operation) noexcept {
		switch (operation) {
			AS(render::LogicOperation::Clear, VK_LOGIC_OP_CLEAR)
			AS(render::LogicOperation::And, VK_LOGIC_OP_AND)
			AS(render::LogicOperation::And_Reverse, VK_LOGIC_OP_AND_REVERSE)
			AS(render::LogicOperation::Copy, VK_LOGIC_OP_COPY)
			AS(render::LogicOperation::And_Inverted, VK_LOGIC_OP_AND_INVERTED)
			AS(render::LogicOperation::No_Operation, VK_LOGIC_OP_NO_OP)
			AS(render::LogicOperation::Xor, VK_LOGIC_OP_XOR)
			AS(render::LogicOperation::Or, VK_LOGIC_OP_OR)
			AS(render::LogicOperation::Nor, VK_LOGIC_OP_NOR)
			AS(render::LogicOperation::Equivalent, VK_LOGIC_OP_EQUIVALENT)
			AS(render::LogicOperation::Invert, VK_LOGIC_OP_INVERT)
			AS(render::LogicOperation::Or_Reverse, VK_LOGIC_OP_OR_REVERSE)
			AS(render::LogicOperation::Copy_Inverted, VK_LOGIC_OP_COPY_INVERTED)
			AS(render::LogicOperation::Or_Inverted, VK_LOGIC_OP_OR_INVERTED)
			AS(render::LogicOperation::Nand, VK_LOGIC_OP_NAND)
			AS(render::LogicOperation::Set, VK_LOGIC_OP_SET)
		}

		return {};
	}

	constexpr inline VkFormat toVK(const render::PixelFormat format) noexcept {
		switch (format) {
			AS(render::PixelFormat::R8_SNorm, VK_FORMAT_R8_SNORM)
			AS(render::PixelFormat::RG8_SNorm, VK_FORMAT_R8G8_SNORM)
			AS(render::PixelFormat::RGB8_SNorm, VK_FORMAT_R8G8B8_SNORM)
			AS(render::PixelFormat::RGBA8_SNorm, VK_FORMAT_R8G8B8A8_SNORM)
			AS(render::PixelFormat::R8_UNorm, VK_FORMAT_R8_UNORM)
			AS(render::PixelFormat::RG8_UNorm, VK_FORMAT_R8G8_UNORM)
			AS(render::PixelFormat::RGB8_UNorm, VK_FORMAT_R8G8B8_UNORM)
			AS(render::PixelFormat::RGBA8_UNorm, VK_FORMAT_R8G8B8A8_UNORM)
			AS(render::PixelFormat::R16_SNorm, VK_FORMAT_R16_SNORM)
			AS(render::PixelFormat::RG16_SNorm, VK_FORMAT_R16G16_SNORM)
			AS(render::PixelFormat::RGB16_SNorm, VK_FORMAT_R16G16B16_SNORM)
			AS(render::PixelFormat::RGBA16_SNorm, VK_FORMAT_R16G16B16A16_SNORM)
			AS(render::PixelFormat::R16_UNorm, VK_FORMAT_R16_UNORM)
			AS(render::PixelFormat::RG16_UNorm, VK_FORMAT_R16G16_UNORM)
			AS(render::PixelFormat::RGB16_UNorm, VK_FORMAT_R16G16B16_UNORM)
			AS(render::PixelFormat::RGBA16_UNorm, VK_FORMAT_R16G16B16A16_UNORM)
			AS(render::PixelFormat::RGB10_A2_UNorm,
			   VK_FORMAT_A2B10G10R10_UNORM_PACK32)
			AS(render::PixelFormat::RGBA4_UNorm,
			   VK_FORMAT_R4G4B4A4_UNORM_PACK16)
			AS(render::PixelFormat::RGB5_A1_UNorm,
			   VK_FORMAT_R5G5B5A1_UNORM_PACK16)
			AS(render::PixelFormat::RGB_5_6_5_UNorm,
			   VK_FORMAT_R5G6B5_UNORM_PACK16)
			AS(render::PixelFormat::BGR8_UNorm, VK_FORMAT_B8G8R8_UNORM)
			AS(render::PixelFormat::BGRA8_UNorm, VK_FORMAT_B8G8R8A8_UNORM)
			AS(render::PixelFormat::R8I, VK_FORMAT_R8_SINT)
			AS(render::PixelFormat::RG8I, VK_FORMAT_R8G8_SINT)
			AS(render::PixelFormat::RGB8I, VK_FORMAT_R8G8B8_SINT)
			AS(render::PixelFormat::RGBA8I, VK_FORMAT_R8G8B8A8_SINT)
			AS(render::PixelFormat::R8U, VK_FORMAT_R8_UINT)
			AS(render::PixelFormat::RG8U, VK_FORMAT_R8G8_UINT)
			AS(render::PixelFormat::RGB8U, VK_FORMAT_R8G8B8_UINT)
			AS(render::PixelFormat::RGBA8U, VK_FORMAT_R8G8B8_UINT)
			AS(render::PixelFormat::R16I, VK_FORMAT_R16_SINT)
			AS(render::PixelFormat::RG16I, VK_FORMAT_R16G16_SINT)
			AS(render::PixelFormat::RGB16I, VK_FORMAT_R16G16B16_SINT)
			AS(render::PixelFormat::RGBA16I, VK_FORMAT_R16G16B16A16_SINT)
			AS(render::PixelFormat::R16U, VK_FORMAT_R16_UINT)
			AS(render::PixelFormat::RG16U, VK_FORMAT_R16G16_UINT)
			AS(render::PixelFormat::RGB16U, VK_FORMAT_R16G16B16_UINT)
			AS(render::PixelFormat::RGBA16U, VK_FORMAT_R16G16B16A16_UINT)
			AS(render::PixelFormat::R32I, VK_FORMAT_R32_SINT)
			AS(render::PixelFormat::RG32I, VK_FORMAT_R32G32_SINT)
			AS(render::PixelFormat::RGB32I, VK_FORMAT_R32G32B32_SINT)
			AS(render::PixelFormat::RGBA32I, VK_FORMAT_R32G32B32A32_SINT)
			AS(render::PixelFormat::R32U, VK_FORMAT_R32_UINT)
			AS(render::PixelFormat::RG32U, VK_FORMAT_R32G32_UINT)
			AS(render::PixelFormat::RGB32U, VK_FORMAT_R32G32B32_UINT)
			AS(render::PixelFormat::RGBA32U, VK_FORMAT_R32G32B32A32_UINT)
			AS(render::PixelFormat::RGB10_A2U,
			   VK_FORMAT_A2B10G10R10_UINT_PACK32)
			AS(render::PixelFormat::R16F, VK_FORMAT_R16_SFLOAT)
			AS(render::PixelFormat::RG16F, VK_FORMAT_R16G16_SFLOAT)
			AS(render::PixelFormat::RGB16F, VK_FORMAT_R16G16B16_SFLOAT)
			AS(render::PixelFormat::RGBA16F, VK_FORMAT_R16G16B16A16_SFLOAT)
			AS(render::PixelFormat::R32F, VK_FORMAT_R32_SFLOAT)
			AS(render::PixelFormat::RG32F, VK_FORMAT_R32G32_SFLOAT)
			AS(render::PixelFormat::RGB32F, VK_FORMAT_R32G32B32_SFLOAT)
			AS(render::PixelFormat::RGBA32F, VK_FORMAT_R32G32B32A32_SFLOAT)
			AS(render::PixelFormat::RGB_11_11_10F,
			   VK_FORMAT_B10G11R11_UFLOAT_PACK32)
			AS(render::PixelFormat::sRGB8, VK_FORMAT_R8G8B8_SRGB)
			AS(render::PixelFormat::sRGBA8, VK_FORMAT_R8G8B8A8_SRGB)
			AS(render::PixelFormat::sBGR8, VK_FORMAT_B8G8R8_SRGB)
			AS(render::PixelFormat::sBGRA8, VK_FORMAT_B8G8R8A8_SRGB)
			AS(render::PixelFormat::Depth16, VK_FORMAT_D16_UNORM)
			AS(render::PixelFormat::Depth24, VK_FORMAT_X8_D24_UNORM_PACK32)
			AS(render::PixelFormat::Depth32F, VK_FORMAT_D32_SFLOAT)
			AS(render::PixelFormat::Depth16_Stencil8,
			   VK_FORMAT_D16_UNORM_S8_UINT)
			AS(render::PixelFormat::Depth24_Stencil8,
			   VK_FORMAT_D24_UNORM_S8_UINT)
			AS(render::PixelFormat::Depth32F_Stencil8,
			   VK_FORMAT_D32_SFLOAT_S8_UINT)
			AS(render::PixelFormat::Undefined, VK_FORMAT_UNDEFINED)
		}

		return VK_FORMAT_UNDEFINED;
	}

	using VkPixelFormatType =
		core::NamedType<VkFormat, struct VkPixelFormatTag>;
	constexpr inline render::PixelFormat
		fromVK(const VkPixelFormatType proxy) noexcept {
		auto format = proxy.get();

		switch (format) {
			FROM(render::PixelFormat::R8_SNorm, VK_FORMAT_R8_SNORM)
			FROM(render::PixelFormat::RG8_SNorm, VK_FORMAT_R8G8_SNORM)
			FROM(render::PixelFormat::RGB8_SNorm, VK_FORMAT_R8G8B8_SNORM)
			FROM(render::PixelFormat::RGBA8_SNorm, VK_FORMAT_R8G8B8A8_SNORM)
			FROM(render::PixelFormat::R8_UNorm, VK_FORMAT_R8_UNORM)
			FROM(render::PixelFormat::RG8_UNorm, VK_FORMAT_R8G8_UNORM)
			FROM(render::PixelFormat::RGB8_UNorm, VK_FORMAT_R8G8B8_UNORM)
			FROM(render::PixelFormat::RGBA8_UNorm, VK_FORMAT_R8G8B8A8_UNORM)
			FROM(render::PixelFormat::R16_SNorm, VK_FORMAT_R16_SNORM)
			FROM(render::PixelFormat::RG16_SNorm, VK_FORMAT_R16G16_SNORM)
			FROM(render::PixelFormat::RGB16_SNorm, VK_FORMAT_R16G16B16_SNORM)
			FROM(render::PixelFormat::RGBA16_SNorm,
				 VK_FORMAT_R16G16B16A16_SNORM)
			FROM(render::PixelFormat::R16_UNorm, VK_FORMAT_R16_UNORM)
			FROM(render::PixelFormat::RG16_UNorm, VK_FORMAT_R16G16_UNORM)
			FROM(render::PixelFormat::RGB16_UNorm, VK_FORMAT_R16G16B16_UNORM)
			FROM(render::PixelFormat::RGBA16_UNorm,
				 VK_FORMAT_R16G16B16A16_UNORM)
			FROM(render::PixelFormat::RGB10_A2_UNorm,
				 VK_FORMAT_A2B10G10R10_UNORM_PACK32)
			FROM(render::PixelFormat::RGBA4_UNorm,
				 VK_FORMAT_R4G4B4A4_UNORM_PACK16)
			FROM(render::PixelFormat::RGB5_A1_UNorm,
				 VK_FORMAT_R5G5B5A1_UNORM_PACK16)
			FROM(render::PixelFormat::RGB_5_6_5_UNorm,
				 VK_FORMAT_R5G6B5_UNORM_PACK16)
			FROM(render::PixelFormat::BGR8_UNorm, VK_FORMAT_B8G8R8_UNORM)
			FROM(render::PixelFormat::BGRA8_UNorm, VK_FORMAT_B8G8R8A8_UNORM)
			FROM(render::PixelFormat::R8I, VK_FORMAT_R8_SINT)
			FROM(render::PixelFormat::RG8I, VK_FORMAT_R8G8_SINT)
			FROM(render::PixelFormat::RGB8I, VK_FORMAT_R8G8B8_SINT)
			FROM(render::PixelFormat::RGBA8I, VK_FORMAT_R8G8B8A8_SINT)
			FROM(render::PixelFormat::R8U, VK_FORMAT_R8_UINT)
			FROM(render::PixelFormat::RG8U, VK_FORMAT_R8G8_UINT)
			FROM(render::PixelFormat::RGB8U, VK_FORMAT_R8G8B8_UINT)
			FROM(render::PixelFormat::RGBA8U, VK_FORMAT_R8G8B8A8_UINT)
			FROM(render::PixelFormat::R16I, VK_FORMAT_R16_SINT)
			FROM(render::PixelFormat::RG16I, VK_FORMAT_R16G16_SINT)
			FROM(render::PixelFormat::RGB16I, VK_FORMAT_R16G16B16_SINT)
			FROM(render::PixelFormat::RGBA16I, VK_FORMAT_R16G16B16A16_SINT)
			FROM(render::PixelFormat::R16U, VK_FORMAT_R16_UINT)
			FROM(render::PixelFormat::RG16U, VK_FORMAT_R16G16_UINT)
			FROM(render::PixelFormat::RGB16U, VK_FORMAT_R16G16B16_UINT)
			FROM(render::PixelFormat::RGBA16U, VK_FORMAT_R16G16B16A16_UINT)
			FROM(render::PixelFormat::R32I, VK_FORMAT_R32_SINT)
			FROM(render::PixelFormat::RG32I, VK_FORMAT_R32G32_SINT)
			FROM(render::PixelFormat::RGB32I, VK_FORMAT_R32G32B32_SINT)
			FROM(render::PixelFormat::RGBA32I, VK_FORMAT_R32G32B32A32_SINT)
			FROM(render::PixelFormat::R32U, VK_FORMAT_R32_UINT)
			FROM(render::PixelFormat::RG32U, VK_FORMAT_R32G32_UINT)
			FROM(render::PixelFormat::RGB32U, VK_FORMAT_R32G32B32_UINT)
			FROM(render::PixelFormat::RGBA32U, VK_FORMAT_R32G32B32A32_UINT)
			FROM(render::PixelFormat::RGB10_A2U,
				 VK_FORMAT_A2B10G10R10_UINT_PACK32)
			FROM(render::PixelFormat::R16F, VK_FORMAT_R16_SFLOAT)
			FROM(render::PixelFormat::RG16F, VK_FORMAT_R16G16_SFLOAT)
			FROM(render::PixelFormat::RGB16F, VK_FORMAT_R16G16B16_SFLOAT)
			FROM(render::PixelFormat::RGBA16F, VK_FORMAT_R16G16B16A16_SFLOAT)
			FROM(render::PixelFormat::R32F, VK_FORMAT_R32_SFLOAT)
			FROM(render::PixelFormat::RG32F, VK_FORMAT_R32G32_SFLOAT)
			FROM(render::PixelFormat::RGB32F, VK_FORMAT_R32G32B32_SFLOAT)
			FROM(render::PixelFormat::RGBA32F, VK_FORMAT_R32G32B32A32_SFLOAT)
			FROM(render::PixelFormat::RGB_11_11_10F,
				 VK_FORMAT_B10G11R11_UFLOAT_PACK32)
			FROM(render::PixelFormat::sRGB8, VK_FORMAT_R8G8B8_SRGB)
			FROM(render::PixelFormat::sRGBA8, VK_FORMAT_R8G8B8A8_SRGB)
			FROM(render::PixelFormat::sBGR8, VK_FORMAT_B8G8R8_SRGB)
			FROM(render::PixelFormat::sBGRA8, VK_FORMAT_B8G8R8A8_SRGB)
			FROM(render::PixelFormat::Depth16, VK_FORMAT_D16_UNORM)
			FROM(render::PixelFormat::Depth24, VK_FORMAT_X8_D24_UNORM_PACK32)
			FROM(render::PixelFormat::Depth32F, VK_FORMAT_D32_SFLOAT)
			FROM(render::PixelFormat::Depth16_Stencil8,
				 VK_FORMAT_D16_UNORM_S8_UINT)
			FROM(render::PixelFormat::Depth24_Stencil8,
				 VK_FORMAT_D24_UNORM_S8_UINT)
			FROM(render::PixelFormat::Depth32F_Stencil8,
				 VK_FORMAT_D32_SFLOAT_S8_UINT)
		default:
			return render::PixelFormat::Undefined;
		}
	}

	constexpr inline VkAttachmentLoadOp
		toVK(const render::AttachmentLoadOperation op) noexcept {
		switch (op) {
			AS(render::AttachmentLoadOperation::Load,
			   VK_ATTACHMENT_LOAD_OP_LOAD)
			AS(render::AttachmentLoadOperation::Clear,
			   VK_ATTACHMENT_LOAD_OP_CLEAR)
			AS(render::AttachmentLoadOperation::Dont_Care,
			   VK_ATTACHMENT_LOAD_OP_DONT_CARE)
		}

		return {};
	}

	constexpr inline render::AttachmentLoadOperation
		fromVK(VkAttachmentLoadOp op) noexcept {
		switch (op) {
			FROM(render::AttachmentLoadOperation::Load,
				 VK_ATTACHMENT_LOAD_OP_LOAD)
			FROM(render::AttachmentLoadOperation::Clear,
				 VK_ATTACHMENT_LOAD_OP_CLEAR)
			FROM(render::AttachmentLoadOperation::Dont_Care,
				 VK_ATTACHMENT_LOAD_OP_DONT_CARE)
		default:
			return {};
		}
	}

	constexpr inline VkAttachmentStoreOp
		toVK(const render::AttachmentStoreOperation op) noexcept {
		switch (op) {
			AS(render::AttachmentStoreOperation::Store,
			   VK_ATTACHMENT_STORE_OP_STORE)
			AS(render::AttachmentStoreOperation::Dont_Care,
			   VK_ATTACHMENT_STORE_OP_DONT_CARE)
		}

		return {};
	}

	constexpr inline render::AttachmentStoreOperation
		fromVK(const VkAttachmentStoreOp op) noexcept {
		switch (op) {
			FROM(render::AttachmentStoreOperation::Store,
				 VK_ATTACHMENT_STORE_OP_STORE)
			FROM(render::AttachmentStoreOperation::Dont_Care,
				 VK_ATTACHMENT_STORE_OP_DONT_CARE)
		default:
			return {};
		}
	}

	constexpr inline VkImageLayout
		toVK(const render::ImageLayout type) noexcept {
		switch (type) {
			AS(render::ImageLayout::Undefined, VK_IMAGE_LAYOUT_UNDEFINED)
			AS(render::ImageLayout::General, VK_IMAGE_LAYOUT_GENERAL)
			AS(render::ImageLayout::Color_Attachment_Optimal,
			   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
			AS(render::ImageLayout::Depth_Stencil_Attachment_Optimal,
			   VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
			AS(render::ImageLayout::Depth_Stencil_Read_Only_Optimal,
			   VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL)
			AS(render::ImageLayout::Shader_Read_Only_Optimal,
			   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			AS(render::ImageLayout::Transfer_Src_Optimal,
			   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
			AS(render::ImageLayout::Transfer_Dst_Optimal,
			   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			AS(render::ImageLayout::Preinitialized,
			   VK_IMAGE_LAYOUT_PREINITIALIZED)
			AS(render::ImageLayout::Depth_Read_Only_Stencil_Attachment_Optimal,
			   VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL)
			AS(render::ImageLayout::Depth_Attachment_Stencil_Read_Only_Optimal,
			   VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL)
			AS(render::ImageLayout::Present_Src,
			   VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
			AS(render::ImageLayout::Shared_Present,
			   VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR)
		}

		return VK_IMAGE_LAYOUT_UNDEFINED;
	}

	constexpr inline render::ImageLayout
		fromVK(const VkImageLayout layout) noexcept {
		switch (layout) {
			FROM(render::ImageLayout::Undefined, VK_IMAGE_LAYOUT_UNDEFINED)
			FROM(render::ImageLayout::General, VK_IMAGE_LAYOUT_GENERAL)
			FROM(render::ImageLayout::Color_Attachment_Optimal,
				 VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
			FROM(render::ImageLayout::Depth_Stencil_Attachment_Optimal,
				 VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
			FROM(render::ImageLayout::Depth_Stencil_Read_Only_Optimal,
				 VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL)
			FROM(render::ImageLayout::Shader_Read_Only_Optimal,
				 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			FROM(render::ImageLayout::Transfer_Src_Optimal,
				 VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
			FROM(render::ImageLayout::Transfer_Dst_Optimal,
				 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			FROM(render::ImageLayout::Preinitialized,
				 VK_IMAGE_LAYOUT_PREINITIALIZED)
			FROM(
				render::ImageLayout::Depth_Read_Only_Stencil_Attachment_Optimal,
				VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL)
			FROM(
				render::ImageLayout::Depth_Attachment_Stencil_Read_Only_Optimal,
				VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL)
			FROM(render::ImageLayout::Present_Src,
				 VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
			FROM(render::ImageLayout::Shared_Present,
				 VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR)
		default:
			return render::ImageLayout::Undefined;
		}
	}

	inline constexpr VkImageAspectFlags
		toVK(render::ImageAspectMask mode) noexcept {
		auto flag = VkShaderStageFlags{};

		FLAG_COMPARE(mode, render::ImageAspectMask::Color,
					 flag |= VK_IMAGE_ASPECT_COLOR_BIT)
		FLAG_COMPARE(mode, render::ImageAspectMask::Depth,
					 flag |= VK_IMAGE_ASPECT_DEPTH_BIT)
		FLAG_COMPARE(mode, render::ImageAspectMask::Stencil,
					 flag |= VK_IMAGE_ASPECT_STENCIL_BIT)

		return flag;
	}

	constexpr inline VkPipelineBindPoint
		toVK(const render::PipelineBindPoint op) noexcept {
		switch (op) {
			AS(render::PipelineBindPoint::Graphics,
			   VK_PIPELINE_BIND_POINT_GRAPHICS)
			AS(render::PipelineBindPoint::Compute,
			   VK_PIPELINE_BIND_POINT_COMPUTE)
		}

		return {};
	}

	constexpr inline render::PipelineBindPoint
		fromVK(const VkPipelineBindPoint op) noexcept {
		switch (op) {
			FROM(render::PipelineBindPoint::Graphics,
				 VK_PIPELINE_BIND_POINT_GRAPHICS)
			FROM(render::PipelineBindPoint::Compute,
				 VK_PIPELINE_BIND_POINT_COMPUTE)
		default:
			return {};
		}
	}

	constexpr inline VkVertexInputRate
		toVK(const render::VertexInputRate op) noexcept {
		switch (op) {
			AS(render::VertexInputRate::Vertex, VK_VERTEX_INPUT_RATE_VERTEX)
			AS(render::VertexInputRate::Instance, VK_VERTEX_INPUT_RATE_INSTANCE)
		}

		return {};
	}

	constexpr inline render::VertexInputRate
		fromVK(const VkVertexInputRate op) noexcept {
		switch (op) {
			FROM(render::VertexInputRate::Vertex, VK_VERTEX_INPUT_RATE_VERTEX)
			FROM(render::VertexInputRate::Instance,
				 VK_VERTEX_INPUT_RATE_INSTANCE)
		default:
			return {};
		}
	}

	constexpr inline VkFormat toVK(const render::Format format) noexcept {
		switch (format) {
			AS(render::Format::Byte, VK_FORMAT_R8_SINT)
			AS(render::Format::Byte2, VK_FORMAT_R8G8_SINT)
			AS(render::Format::Byte3, VK_FORMAT_R8G8B8_SINT)
			AS(render::Format::Byte4, VK_FORMAT_R8G8B8A8_SINT)
			AS(render::Format::Byte_Norm, VK_FORMAT_R8_SNORM)
			AS(render::Format::Byte2_Norm, VK_FORMAT_R8G8_SNORM)
			AS(render::Format::Byte3_Norm, VK_FORMAT_R8G8B8_SNORM)
			AS(render::Format::Byte4_Norm, VK_FORMAT_R8G8B8A8_SNORM)
			AS(render::Format::Byte_Scaled, VK_FORMAT_R8_SSCALED)
			AS(render::Format::Byte2_Scaled, VK_FORMAT_R8G8_SSCALED)
			AS(render::Format::Byte3_Scaled, VK_FORMAT_R8G8B8_SSCALED)
			AS(render::Format::Byte4_Scaled, VK_FORMAT_R8G8B8A8_SSCALED)
			AS(render::Format::UByte, VK_FORMAT_R8_UINT)
			AS(render::Format::UByte2, VK_FORMAT_R8G8_UINT)
			AS(render::Format::UByte3, VK_FORMAT_R8G8B8_UINT)
			AS(render::Format::UByte4, VK_FORMAT_R8G8B8A8_UINT)
			AS(render::Format::UByte_Norm, VK_FORMAT_R8_UNORM)
			AS(render::Format::UByte2_Norm, VK_FORMAT_R8G8_UNORM)
			AS(render::Format::UByte3_Norm, VK_FORMAT_R8G8B8_UNORM)
			AS(render::Format::UByte4_Norm, VK_FORMAT_R8G8B8A8_UNORM)
			AS(render::Format::UByte_Scaled, VK_FORMAT_R8_USCALED)
			AS(render::Format::UByte2_Scaled, VK_FORMAT_R8G8_USCALED)
			AS(render::Format::UByte3_Scaled, VK_FORMAT_R8G8B8_USCALED)
			AS(render::Format::UByte4_Scaled, VK_FORMAT_R8G8B8A8_USCALED)
			AS(render::Format::Short, VK_FORMAT_R16_SINT)
			AS(render::Format::Short2, VK_FORMAT_R16G16_SINT)
			AS(render::Format::Short3, VK_FORMAT_R16G16B16_SINT)
			AS(render::Format::Short4, VK_FORMAT_R16G16B16A16_SINT)
			AS(render::Format::Short_Norm, VK_FORMAT_R16_SNORM)
			AS(render::Format::Short2_Norm, VK_FORMAT_R16G16_SNORM)
			AS(render::Format::Short3_Norm, VK_FORMAT_R16G16B16_SNORM)
			AS(render::Format::Short4_Norm, VK_FORMAT_R16G16B16A16_SNORM)
			AS(render::Format::Short_Scaled, VK_FORMAT_R16_SSCALED)
			AS(render::Format::Short2_Scaled, VK_FORMAT_R16G16_SSCALED)
			AS(render::Format::Short3_Scaled, VK_FORMAT_R16G16B16_SSCALED)
			AS(render::Format::Short4_Scaled, VK_FORMAT_R16G16B16A16_SSCALED)
			AS(render::Format::UShort, VK_FORMAT_R16_UINT)
			AS(render::Format::UShort2, VK_FORMAT_R16G16_UINT)
			AS(render::Format::UShort3, VK_FORMAT_R16G16B16_UINT)
			AS(render::Format::UShort4, VK_FORMAT_R16G16B16A16_UINT)
			AS(render::Format::UShort_Norm, VK_FORMAT_R16_UNORM)
			AS(render::Format::UShort2_Norm, VK_FORMAT_R16G16_UNORM)
			AS(render::Format::UShort3_Norm, VK_FORMAT_R16G16B16_UNORM)
			AS(render::Format::UShort4_Norm, VK_FORMAT_R16G16B16A16_UNORM)
			AS(render::Format::UShort_Scaled, VK_FORMAT_R16_USCALED)
			AS(render::Format::UShort2_Scaled, VK_FORMAT_R16G16_USCALED)
			AS(render::Format::UShort3_Scaled, VK_FORMAT_R16G16B16_USCALED)
			AS(render::Format::UShort4_Scaled, VK_FORMAT_R16G16B16A16_USCALED)
			AS(render::Format::Int, VK_FORMAT_R32_SINT)
			AS(render::Format::Int2, VK_FORMAT_R32G32_SINT)
			AS(render::Format::Int3, VK_FORMAT_R32G32B32_SINT)
			AS(render::Format::Int4, VK_FORMAT_R32G32B32A32_SINT)
			AS(render::Format::UInt, VK_FORMAT_R32_UINT)
			AS(render::Format::UInt2, VK_FORMAT_R32G32_UINT)
			AS(render::Format::UInt3, VK_FORMAT_R32G32B32_UINT)
			AS(render::Format::UInt4, VK_FORMAT_R32G32B32A32_UINT)
			AS(render::Format::Long, VK_FORMAT_R64_SINT)
			AS(render::Format::Long2, VK_FORMAT_R64G64_SINT)
			AS(render::Format::Long3, VK_FORMAT_R64G64B64_SINT)
			AS(render::Format::Long4, VK_FORMAT_R64G64B64A64_SINT)
			AS(render::Format::ULong, VK_FORMAT_R64_UINT)
			AS(render::Format::ULong2, VK_FORMAT_R64G64_UINT)
			AS(render::Format::ULong3, VK_FORMAT_R64G64B64_UINT)
			AS(render::Format::ULong4, VK_FORMAT_R64G64B64A64_UINT)
			AS(render::Format::Float, VK_FORMAT_R32_SFLOAT)
			AS(render::Format::Float2, VK_FORMAT_R32G32_SFLOAT)
			AS(render::Format::Float3, VK_FORMAT_R32G32B32_SFLOAT)
			AS(render::Format::Float4, VK_FORMAT_R32G32B32A32_SFLOAT)
			AS(render::Format::Double, VK_FORMAT_R64_SFLOAT)
			AS(render::Format::Double2, VK_FORMAT_R64G64_SFLOAT)
			AS(render::Format::Double3, VK_FORMAT_R64G64B64_SFLOAT)
			AS(render::Format::Double4, VK_FORMAT_R64G64B64A64_SFLOAT)
			AS(render::Format::Undefined, VK_FORMAT_UNDEFINED)
		}

		return VK_FORMAT_UNDEFINED;
	}

	using VkFormatType = core::NamedType<VkFormat, struct VkFormatTag>;
	constexpr inline render::Format fromVK(const VkFormatType proxy) noexcept {
		auto format = proxy.get();
		switch (format) {
			FROM(render::Format::Byte, VK_FORMAT_R8_SINT)
			FROM(render::Format::Byte2, VK_FORMAT_R8G8_SINT)
			FROM(render::Format::Byte3, VK_FORMAT_R8G8B8_SINT)
			FROM(render::Format::Byte4, VK_FORMAT_R8G8B8A8_SINT)
			FROM(render::Format::Byte_Norm, VK_FORMAT_R8_SNORM)
			FROM(render::Format::Byte2_Norm, VK_FORMAT_R8G8_SNORM)
			FROM(render::Format::Byte3_Norm, VK_FORMAT_R8G8B8_SNORM)
			FROM(render::Format::Byte4_Norm, VK_FORMAT_R8G8B8A8_SNORM)
			FROM(render::Format::Byte_Scaled, VK_FORMAT_R8_SSCALED)
			FROM(render::Format::Byte2_Scaled, VK_FORMAT_R8G8_SSCALED)
			FROM(render::Format::Byte3_Scaled, VK_FORMAT_R8G8B8_SSCALED)
			FROM(render::Format::Byte4_Scaled, VK_FORMAT_R8G8B8A8_SSCALED)
			FROM(render::Format::UByte, VK_FORMAT_R8_UINT)
			FROM(render::Format::UByte2, VK_FORMAT_R8G8_UINT)
			FROM(render::Format::UByte3, VK_FORMAT_R8G8B8_UINT)
			FROM(render::Format::UByte4, VK_FORMAT_R8G8B8A8_UINT)
			FROM(render::Format::UByte_Norm, VK_FORMAT_R8_UNORM)
			FROM(render::Format::UByte2_Norm, VK_FORMAT_R8G8_UNORM)
			FROM(render::Format::UByte3_Norm, VK_FORMAT_R8G8B8_UNORM)
			FROM(render::Format::UByte4_Norm, VK_FORMAT_R8G8B8A8_UNORM)
			FROM(render::Format::UByte_Scaled, VK_FORMAT_R8_USCALED)
			FROM(render::Format::UByte2_Scaled, VK_FORMAT_R8G8_USCALED)
			FROM(render::Format::UByte3_Scaled, VK_FORMAT_R8G8B8_USCALED)
			FROM(render::Format::UByte4_Scaled, VK_FORMAT_R8G8B8A8_USCALED)
			FROM(render::Format::Short, VK_FORMAT_R16_SINT)
			FROM(render::Format::Short2, VK_FORMAT_R16G16_SINT)
			FROM(render::Format::Short3, VK_FORMAT_R16G16B16_SINT)
			FROM(render::Format::Short4, VK_FORMAT_R16G16B16A16_SINT)
			FROM(render::Format::Short_Norm, VK_FORMAT_R16_SNORM)
			FROM(render::Format::Short2_Norm, VK_FORMAT_R16G16_SNORM)
			FROM(render::Format::Short3_Norm, VK_FORMAT_R16G16B16_SNORM)
			FROM(render::Format::Short4_Norm, VK_FORMAT_R16G16B16A16_SNORM)
			FROM(render::Format::Short_Scaled, VK_FORMAT_R16_SSCALED)
			FROM(render::Format::Short2_Scaled, VK_FORMAT_R16G16_SSCALED)
			FROM(render::Format::Short3_Scaled, VK_FORMAT_R16G16B16_SSCALED)
			FROM(render::Format::Short4_Scaled, VK_FORMAT_R16G16B16A16_SSCALED)
			FROM(render::Format::UShort, VK_FORMAT_R16_UINT)
			FROM(render::Format::UShort2, VK_FORMAT_R16G16_UINT)
			FROM(render::Format::UShort3, VK_FORMAT_R16G16B16_UINT)
			FROM(render::Format::UShort4, VK_FORMAT_R16G16B16A16_UINT)
			FROM(render::Format::UShort_Norm, VK_FORMAT_R16_UNORM)
			FROM(render::Format::UShort2_Norm, VK_FORMAT_R16G16_UNORM)
			FROM(render::Format::UShort3_Norm, VK_FORMAT_R16G16B16_UNORM)
			FROM(render::Format::UShort4_Norm, VK_FORMAT_R16G16B16A16_UNORM)
			FROM(render::Format::UShort_Scaled, VK_FORMAT_R16_USCALED)
			FROM(render::Format::UShort2_Scaled, VK_FORMAT_R16G16_USCALED)
			FROM(render::Format::UShort3_Scaled, VK_FORMAT_R16G16B16_USCALED)
			FROM(render::Format::UShort4_Scaled, VK_FORMAT_R16G16B16A16_USCALED)
			FROM(render::Format::Int, VK_FORMAT_R32_SINT)
			FROM(render::Format::Int2, VK_FORMAT_R32G32_SINT)
			FROM(render::Format::Int3, VK_FORMAT_R32G32B32_SINT)
			FROM(render::Format::Int4, VK_FORMAT_R32G32B32A32_SINT)
			FROM(render::Format::UInt, VK_FORMAT_R32_UINT)
			FROM(render::Format::UInt2, VK_FORMAT_R32G32_UINT)
			FROM(render::Format::UInt3, VK_FORMAT_R32G32B32_UINT)
			FROM(render::Format::UInt4, VK_FORMAT_R32G32B32A32_UINT)
			FROM(render::Format::Long, VK_FORMAT_R64_SINT)
			FROM(render::Format::Long2, VK_FORMAT_R64G64_SINT)
			FROM(render::Format::Long3, VK_FORMAT_R64G64B64_SINT)
			FROM(render::Format::Long4, VK_FORMAT_R64G64B64A64_SINT)
			FROM(render::Format::ULong, VK_FORMAT_R64_UINT)
			FROM(render::Format::ULong2, VK_FORMAT_R64G64_UINT)
			FROM(render::Format::ULong3, VK_FORMAT_R64G64B64_UINT)
			FROM(render::Format::ULong4, VK_FORMAT_R64G64B64A64_UINT)
			FROM(render::Format::Float, VK_FORMAT_R32_SFLOAT)
			FROM(render::Format::Float2, VK_FORMAT_R32G32_SFLOAT)
			FROM(render::Format::Float3, VK_FORMAT_R32G32B32_SFLOAT)
			FROM(render::Format::Float4, VK_FORMAT_R32G32B32A32_SFLOAT)
			FROM(render::Format::Double, VK_FORMAT_R64_SFLOAT)
			FROM(render::Format::Double2, VK_FORMAT_R64G64_SFLOAT)
			FROM(render::Format::Double3, VK_FORMAT_R64G64B64_SFLOAT)
			FROM(render::Format::Double4, VK_FORMAT_R64G64B64A64_SFLOAT)
			FROM(render::Format::Undefined, VK_FORMAT_UNDEFINED)
		default:
			return Format::Undefined;
		}
	}

	inline constexpr VkBufferUsageFlags
		toVK(render::HardwareBufferUsage flags) noexcept {
		auto flag = 0;

		FLAG_COMPARE(flags, render::HardwareBufferUsage::Vertex,
					 flag |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
		FLAG_COMPARE(flags, render::HardwareBufferUsage::Index,
					 flag |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
		FLAG_COMPARE(flags, render::HardwareBufferUsage::Uniform,
					 flag |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
		FLAG_COMPARE(flags, render::HardwareBufferUsage::Transfert_Src,
					 flag |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
		FLAG_COMPARE(flags, render::HardwareBufferUsage::Transfert_Dst,
					 flag |= VK_BUFFER_USAGE_TRANSFER_DST_BIT)

		return static_cast<VkSampleCountFlagBits>(flag);
	}

	inline constexpr VkImageUsageFlags toVK(render::ImageUsage flags) noexcept {
		auto flag = 0;

		FLAG_COMPARE(flags, render::ImageUsage::Transfert_Src,
					 flag |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
		FLAG_COMPARE(flags, render::ImageUsage::Transfert_Dst,
					 flag |= VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		FLAG_COMPARE(flags, render::ImageUsage::Sampled,
					 flag |= VK_IMAGE_USAGE_SAMPLED_BIT)
		FLAG_COMPARE(flags, render::ImageUsage::Storage,
					 flag |= VK_IMAGE_USAGE_STORAGE_BIT)
		FLAG_COMPARE(flags, render::ImageUsage::Color_Attachment,
					 flag |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
		FLAG_COMPARE(flags, render::ImageUsage::Depth_Stencil_Attachment,
					 flag |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
		FLAG_COMPARE(flags, render::ImageUsage::Transient_Attachment,
					 flag |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT)
		FLAG_COMPARE(flags, render::ImageUsage::Input_Attachment,
					 flag |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT)

		return static_cast<VkSampleCountFlagBits>(flag);
	}

	inline constexpr VkMemoryPropertyFlags
		toVK(render::MemoryProperty flags) noexcept {
		auto flag = 0;

		FLAG_COMPARE(flags, render::MemoryProperty::Device_Local,
					 flag |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
		FLAG_COMPARE(flags, render::MemoryProperty::Host_Visible,
					 flag |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
		FLAG_COMPARE(flags, render::MemoryProperty::Host_Coherent,
					 flag |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
		FLAG_COMPARE(flags, render::MemoryProperty::Host_Cached,
					 flag |= VK_MEMORY_PROPERTY_HOST_CACHED_BIT)

		return static_cast<VkSampleCountFlagBits>(flag);
	}

	inline constexpr VkDescriptorType
		toVK(render::DescriptorType mode) noexcept {
		switch (mode) {
			AS(render::DescriptorType::UniformBuffer,
			   VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
			AS(render::DescriptorType::Combined_Image_Sampler,
			   VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
		}

		return {};
	}

	inline constexpr VkCommandBufferLevel
		toVK(render::CommandBufferLevel level) noexcept {
		switch (level) {
			AS(render::CommandBufferLevel::Primary,
			   VK_COMMAND_BUFFER_LEVEL_PRIMARY)
			AS(render::CommandBufferLevel::Secondary,
			   VK_COMMAND_BUFFER_LEVEL_SECONDARY)
		}

		return {};
	}

	inline constexpr VkCompareOp toVK(render::CompareOperation op) noexcept {
		switch (op) {
			AS(render::CompareOperation::Never, VK_COMPARE_OP_NEVER)
			AS(render::CompareOperation::Less, VK_COMPARE_OP_LESS)
			AS(render::CompareOperation::Equal, VK_COMPARE_OP_EQUAL)
			AS(render::CompareOperation::Less_Or_Equal,
			   VK_COMPARE_OP_LESS_OR_EQUAL)
			AS(render::CompareOperation::Greater, VK_COMPARE_OP_GREATER)
			AS(render::CompareOperation::Not_Equal, VK_COMPARE_OP_NOT_EQUAL)
			AS(render::CompareOperation::Greater_Or_Equal,
			   VK_COMPARE_OP_GREATER_OR_EQUAL)
			AS(render::CompareOperation::Always, VK_COMPARE_OP_ALWAYS)
		}

		return {};
	}

	inline constexpr VkFilter toVK(render::Filter filter) noexcept {
		switch (filter) {
			AS(render::Filter::Linear, VK_FILTER_LINEAR)
			AS(render::Filter::Nearest, VK_FILTER_NEAREST)
		}

		return {};
	}

	inline constexpr VkSamplerAddressMode
		toVK(render::SamplerAddressMode mode) noexcept {
		switch (mode) {
			AS(render::SamplerAddressMode::Repeat,
			   VK_SAMPLER_ADDRESS_MODE_REPEAT)
			AS(render::SamplerAddressMode::Mirrored_Repeat,
			   VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT)
			AS(render::SamplerAddressMode::Clamp_To_Edge,
			   VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)
			AS(render::SamplerAddressMode::Clamp_To_Border,
			   VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER)
			AS(render::SamplerAddressMode::Mirror_Clamp_To_Edge,
			   VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE)
		}

		return {};
	}

	inline constexpr VkBorderColor toVK(render::BorderColor color) noexcept {
		switch (color) {
			AS(render::BorderColor::Float_Transparent_Black,
			   VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK)
			AS(render::BorderColor::Int_Transparent_Black,
			   VK_BORDER_COLOR_INT_TRANSPARENT_BLACK)
			AS(render::BorderColor::Float_Opaque_Black,
			   VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK)
			AS(render::BorderColor::Int_Opaque_Black,
			   VK_BORDER_COLOR_INT_OPAQUE_BLACK)
			AS(render::BorderColor::Float_Opaque_White,
			   VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE)
			AS(render::BorderColor::Int_opaque_White,
			   VK_BORDER_COLOR_INT_OPAQUE_WHITE)
		}

		return {};
	}

	inline constexpr VkSamplerMipmapMode
		toVK(render::SamplerMipmapMode filter) noexcept {
		switch (filter) {
			AS(render::SamplerMipmapMode::Linear, VK_SAMPLER_MIPMAP_MODE_LINEAR)
			AS(render::SamplerMipmapMode::Nearest,
			   VK_SAMPLER_MIPMAP_MODE_NEAREST)
		}

		return {};
    }
} // namespace storm::render

#undef FLAG_COMPARE
#undef AS
#undef FROM
