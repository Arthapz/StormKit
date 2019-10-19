// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <cstdint>

#include <storm/core/Flags.hpp>
#include <storm/core/Strings.hpp>

#include <storm/render/core/Vulkan.hpp>

namespace storm::render {
	enum class PhysicalDeviceType : std::uint8_t {
		Discrete_GPU   = 0,
        Virtual_GPU	   = 1,
		Integrated_GPU = 2,
		CPU			   = 3
	};

	inline std::string to_string(PhysicalDeviceType type);

	enum class QueueFlag : std::uint8_t {
		Graphics	   = 1,
		Compute		   = 2,
        Transfert	   = 4,
		Sparse_Binding = 8
	};

	enum class ShaderType : std::uint8_t {
        Vertex	 = 1,
		Fragment = 2,
		Geometry = 4,
        Compute	 = 8
	};

	enum class PrimitiveTopology : std::uint8_t {
        Point_List	   = 0,
        Line_List	   = 1,
        Line_Strip	   = 2,
		Triangle_List  = 3,
		Triangle_Strip = 4,
		Triangle_Fan   = 5
	};

	enum class PolygonMode : std::uint8_t { Fill = 0, Line = 1, Point = 2 };

	enum class CullMode : std::uint8_t {
		None	   = 0,
        Front	   = 1,
		Back	   = 2,
		Front_Back = Front | Back
	};

	enum class FrontFace : std::uint8_t {
		Clockwise		  = 0,
		Counter_Clockwise = 1
	};

	enum class SampleCountFlag : std::uint8_t {
        C1_BIT	= 1,
        C2_BIT	= 2,
        C4_BIT	= 4,
        C8_BIT	= 8,
		C16_BIT = 16,
		C32_BIT = 32,
		C64_BIT = 64
	};

	enum class ColorComponent : std::uint8_t {
        R	 = 1,
        G	 = 2,
        B	 = 4,
        A	 = 8,
        RG	 = R | G,
        RGB	 = RG | B,
		RGBA = RGB | A
	};

	enum class BlendFactor : std::uint8_t {
		One						 = 0,
		Zero					 = 1,
		Src_Color				 = 2,
		One_Minus_Src_Color		 = 3,
		Dst_Color				 = 4,
		One_Minus_Dst_Color		 = 5,
		Src_Alpha				 = 6,
		One_Minus_Src_Alpha		 = 7,
		Dst_Alpha				 = 8,
		One_Minus_Dst_Alpha		 = 9,
		Constant_Color			 = 10,
		One_Minus_Constant_Color = 11,
		Constant_Alpha			 = 12,
		One_Minus_Constant_Alpha = 13,
		Src_Alpha_Saturate		 = 14,
		Src1_Color				 = 15,
		One_Minus_Src1_Color	 = 16,
		Src1_Alpha				 = 17,
		One_Minus_Src1_Alpha	 = 18
	};

	enum class BlendOperation : std::uint8_t {
		Add				  = 0,
		Substract		  = 1,
		Reverse_Substract = 2,
		Min				  = 3,
		Max				  = 4
	};

	enum class LogicOperation : std::uint8_t {
		Clear		  = 0,
		And			  = 1,
        And_Reverse	  = 2,
		Copy		  = 3,
		And_Inverted  = 4,
		No_Operation  = 5,
		Xor			  = 6,
		Or			  = 7,
		Nor			  = 8,
        Equivalent	  = 9,
		Invert		  = 10,
        Or_Reverse	  = 11,
		Copy_Inverted = 12,
        Or_Inverted	  = 13,
		Nand		  = 14,
		Set			  = 15
	};

	enum class PixelFormat : std::uint8_t {
		R8_SNorm		  = 0,
		RG8_SNorm		  = 1,
		RGB8_SNorm		  = 2,
		RGBA8_SNorm		  = 3,
		R8_UNorm		  = 4,
		RG8_UNorm		  = 5,
		RGB8_UNorm		  = 6,
		RGBA8_UNorm		  = 7,
		R16_SNorm		  = 8,
		RG16_SNorm		  = 9,
		RGB16_SNorm		  = 10,
		RGBA16_SNorm	  = 11,
		R16_UNorm		  = 12,
		RG16_UNorm		  = 13,
		RGB16_UNorm		  = 14,
		RGBA16_UNorm	  = 15,
        RGB10_A2_UNorm	  = 16,
		RGBA4_UNorm		  = 17,
        RGB5_A1_UNorm	  = 18,
        RGB_5_6_5_UNorm	  = 19,
		BGR8_UNorm		  = 20,
		BGRA8_UNorm		  = 21,
		R8I				  = 22,
		RG8I			  = 23,
		RGB8I			  = 24,
		RGBA8I			  = 25,
		R8U				  = 26,
		RG8U			  = 27,
		RGB8U			  = 28,
		RGBA8U			  = 29,
		R16I			  = 30,
		RG16I			  = 31,
		RGB16I			  = 32,
		RGBA16I			  = 33,
		R16U			  = 34,
		RG16U			  = 35,
		RGB16U			  = 36,
		RGBA16U			  = 37,
		R32I			  = 38,
		RG32I			  = 39,
		RGB32I			  = 40,
		RGBA32I			  = 41,
		R32U			  = 42,
		RG32U			  = 43,
		RGB32U			  = 44,
		RGBA32U			  = 45,
        RGB10_A2U		  = 46,
		R16F			  = 47,
		RG16F			  = 48,
		RGB16F			  = 49,
		RGBA16F			  = 50,
		R32F			  = 51,
		RG32F			  = 52,
		RGB32F			  = 53,
		RGBA32F			  = 54,
        RGB_11_11_10F	  = 55,
		sRGB8			  = 56,
		sRGBA8			  = 57,
		sBGR8			  = 58,
		sBGRA8			  = 59,
		Depth16			  = 60,
		Depth24			  = 61,
		Depth32F		  = 62,
		Depth16_Stencil8  = 63,
		Depth24_Stencil8  = 64,
		Depth32F_Stencil8 = 65,
		Undefined		  = 254
	};

	enum class AttachmentLoadOperation : std::uint8_t {
		Load	  = 0,
        Clear	  = 1,
		Dont_Care = 2
	};

	enum class AttachmentStoreOperation : std::uint8_t {
        Store	  = 0,
		Dont_Care = 1
	};

	enum class PipelineBindPoint : std::uint8_t { Graphics = 0, Compute = 1 };

	enum class ImageLayout : std::uint8_t {
		General									   = 0,
		Color_Attachment_Optimal				   = 1,
		Depth_Stencil_Attachment_Optimal		   = 2,
		Depth_Stencil_Read_Only_Optimal			   = 3,
		Shader_Read_Only_Optimal				   = 4,
		Transfer_Src_Optimal					   = 5,
		Transfer_Dst_Optimal					   = 6,
		Preinitialized							   = 7,
		Depth_Read_Only_Stencil_Attachment_Optimal = 8,
		Depth_Attachment_Stencil_Read_Only_Optimal = 9,
		Present_Src								   = 10,
		Shared_Present							   = 11,
		Undefined								   = 254,
	};

	enum class ImageAspectMask : std::uint8_t {
        Color	= 1,
        Depth	= 2,
		Stencil = 4
	};

	enum class VertexInputRate : std::uint8_t { Vertex = 0, Instance = 1 };

	enum class Format : std::uint8_t {
		Byte		   = 0,
		Byte2		   = 1,
		Byte3		   = 2,
		Byte4		   = 3,
        Byte_Norm	   = 4,
        Byte2_Norm	   = 5,
        Byte3_Norm	   = 6,
        Byte4_Norm	   = 7,
        Byte_Scaled	   = 8,
		Byte2_Scaled   = 9,
		Byte3_Scaled   = 10,
		Byte4_Scaled   = 11,
		UByte		   = 12,
		UByte2		   = 13,
		UByte3		   = 14,
		UByte4		   = 15,
        UByte_Norm	   = 16,
        UByte2_Norm	   = 17,
        UByte3_Norm	   = 18,
        UByte4_Norm	   = 19,
		UByte_Scaled   = 20,
		UByte2_Scaled  = 21,
		UByte3_Scaled  = 22,
		UByte4_Scaled  = 23,
		Short		   = 24,
		Short2		   = 25,
		Short3		   = 26,
		Short4		   = 27,
        Short_Norm	   = 28,
        Short2_Norm	   = 29,
        Short3_Norm	   = 30,
        Short4_Norm	   = 31,
		Short_Scaled   = 32,
		Short2_Scaled  = 33,
		Short3_Scaled  = 34,
		Short4_Scaled  = 35,
		UShort		   = 36,
		UShort2		   = 37,
		UShort3		   = 38,
		UShort4		   = 39,
        UShort_Norm	   = 40,
		UShort2_Norm   = 41,
		UShort3_Norm   = 42,
		UShort4_Norm   = 43,
		UShort_Scaled  = 44,
		UShort2_Scaled = 45,
		UShort3_Scaled = 46,
		UShort4_Scaled = 47,
		Int			   = 48,
		Int2		   = 49,
		Int3		   = 50,
		Int4		   = 51,
		UInt		   = 52,
		UInt2		   = 53,
		UInt3		   = 54,
		UInt4		   = 55,
		Long		   = 56,
		Long2		   = 57,
		Long3		   = 58,
		Long4		   = 59,
		ULong		   = 60,
		ULong2		   = 61,
		ULong3		   = 62,
		ULong4		   = 63,
		Float		   = 64,
		Float2		   = 65,
		Float3		   = 66,
		Float4		   = 67,
		Double		   = 68,
		Double2		   = 69,
		Double3		   = 70,
		Double4		   = 71,
        Undefined	   = 254
	};

	enum class HardwareBufferUsage : std::uint8_t {
		Vertex		  = 1,
		Index		  = 2,
		Uniform		  = 4,
		Transfert_Src = 8,
		Transfert_Dst = 16
	};

	enum class ImageUsage : std::uint8_t {
		Transfert_Src			 = 1,
		Transfert_Dst			 = 2,
		Sampled					 = 4,
		Storage					 = 8,
		Color_Attachment		 = 16,
		Depth_Stencil_Attachment = 32,
		Transient_Attachment	 = 64,
		Input_Attachment		 = 128
	};

	enum class MemoryProperty : std::uint8_t {
		Device_Local  = 1,
		Host_Visible  = 2,
		Host_Coherent = 4,
        Host_Cached	  = 8
	};

	enum class CommandBufferLevel : std::uint8_t { Primary, Secondary };

	enum class DescriptorType : std::uint8_t {
		UniformBuffer		   = 0,
		Combined_Image_Sampler = 1
	};

	enum class CompareOperation : std::uint8_t {
		Never			 = 0,
		Less			 = 1,
		Equal			 = 2,
        Less_Or_Equal	 = 3,
		Greater			 = 4,
		Not_Equal		 = 5,
		Greater_Or_Equal = 6,
		Always			 = 7
	};

	enum class Filter : std::uint8_t {
		Nearest = 0,
        Linear	= 1,
	};

	enum class SamplerAddressMode : std::uint8_t {
		Repeat				 = 0,
		Mirrored_Repeat		 = 1,
		Clamp_To_Edge		 = 2,
		Clamp_To_Border		 = 3,
		Mirror_Clamp_To_Edge = 4
	};

	enum class BorderColor : std::uint8_t {
		Float_Transparent_Black = 0,
        Int_Transparent_Black	= 1,
		Float_Opaque_Black		= 2,
		Int_Opaque_Black		= 3,
		Float_Opaque_White		= 4,
		Int_opaque_White		= 5
	};

    enum class SamplerMipmapMode : std::uint8_t { Nearest = 0, Linear = 1 };

    enum class WaitResult : std::uint8_t {
        Success = 0,
        Timeout = 1,
        Error	= 2
    };

	constexpr inline bool isDepthOnlyFormat(PixelFormat format) noexcept {
		return format == PixelFormat::Depth16 ||
			   format == PixelFormat::Depth24 ||
			   format == PixelFormat::Depth32F;
	}

	constexpr inline bool isDepthStencilFormat(PixelFormat format) noexcept {
		return format == PixelFormat::Depth16_Stencil8 ||
			   format == PixelFormat::Depth24_Stencil8 ||
			   format == PixelFormat::Depth32F_Stencil8;
	}

	constexpr inline bool isDepthFormat(PixelFormat format) noexcept {
		return isDepthOnlyFormat(format) || isDepthStencilFormat(format);
	}
} // namespace storm::render

FLAG_ENUM(storm::render::QueueFlag)
FLAG_ENUM(storm::render::ShaderType)
FLAG_ENUM(storm::render::SampleCountFlag)
FLAG_ENUM(storm::render::ColorComponent)
FLAG_ENUM(storm::render::ImageAspectMask)
FLAG_ENUM(storm::render::CullMode)
FLAG_ENUM(storm::render::HardwareBufferUsage)
FLAG_ENUM(storm::render::ImageUsage)
FLAG_ENUM(storm::render::MemoryProperty)

#include "Enums.inl"
