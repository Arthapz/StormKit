// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <fstream>
#include <streambuf>

#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>

#include <storm/log/LogHandler.hpp>

#include <storm/render/core/Device.hpp>
#include <storm/render/core/PhysicalDevice.hpp>

#include <storm/render/resource/Shader.hpp>

using namespace storm;
using namespace storm::render;
using namespace storm::log;

constexpr EShLanguage getShaderKind(ShaderType type) {
	switch (type) {
	case ShaderType::Vertex:
		return EShLanguage::EShLangVertex;
	case ShaderType::Fragment:
		return EShLanguage::EShLangFragment;
	case ShaderType::Compute:
		return EShLanguage::EShLangCompute;
	case ShaderType::Geometry:
		return EShLanguage::EShLangGeometry;
	}
}

static auto glslang_initialized = false;

/////////////////////////////////////
/////////////////////////////////////
Shader::Shader(core::filesystem::path filepath, Language language,
			   ShaderType type, const Device &device)
	: m_device{&device}, m_type{type} {
	if (!glslang_initialized) {
		glslang::InitializeProcess();
		glslang_initialized = true;
	}

	auto stream =
		std::ifstream{filepath.string()};
	auto data = std::string{
		(std::istreambuf_iterator<char>{stream}),
		std::istreambuf_iterator<char>{}
	};

	transpileToSpirv(language, {reinterpret_cast<const std::byte *>(std::data(data)), gsl::narrow_cast<core::span<char>::index_type>(std::size(data))});
	compile();
}

/////////////////////////////////////
/////////////////////////////////////
Shader::Shader(core::span<const std::byte> data, Language language,
			   ShaderType type, const Device &device)
	: m_device{&device}, m_type{type} {
	if (!glslang_initialized) {
		glslang::InitializeProcess();
		glslang_initialized = true;
	}

    transpileToSpirv(language, data);
	compile();
}

/////////////////////////////////////
/////////////////////////////////////
Shader::~Shader() {
	if (m_vk_shader_module != VK_NULL_HANDLE)
		m_device->destroyVkShaderModule(m_vk_shader_module);
}

/////////////////////////////////////
/////////////////////////////////////
Shader::Shader(Shader &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Shader &Shader::operator=(Shader &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void Shader::compile() noexcept {
	const auto create_info = VkShaderModuleCreateInfo{
		.sType	= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = std::size(m_source),
		.pCode = reinterpret_cast<const std::uint32_t *>(std::data(m_source))};

	m_vk_shader_module = m_device->createVkShaderModule(create_info);
}

/////////////////////////////////////
/////////////////////////////////////
void Shader::transpileToSpirv(Language language,
							  gsl::span<const std::byte> source) {
	using SpvID = std::uint32_t;

	static constexpr auto client_input_semantics_version = 100;
	static constexpr auto vulkan_client_version = glslang::EShTargetVulkan_1_0;
	static constexpr auto spirv_target_version  = glslang::EShTargetSpv_1_0;
	static constexpr auto messages              = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules | EShMsgDebugInfo);

	if (language == Language::SPIRV) {
		m_source.resize(std::size(source));
		std::copy(std::cbegin(source), std::cend(source), std::begin(m_source));
	} else if (language == Language::GLSL) {
		const auto * const c_source =
			reinterpret_cast<const char * const>(std::data(source));

		auto shader_kind = getShaderKind(m_type);

		auto resources = TBuiltInResource{};
		initResources(resources);

		auto shader = glslang::TShader{shader_kind};
		shader.setStrings(&c_source, 1);
		shader.setEnvInput(glslang::EShSourceGlsl, shader_kind, glslang::EShClientVulkan, client_input_semantics_version);
		shader.setEnvClient(glslang::EShClientVulkan, vulkan_client_version);
		shader.setEnvTarget(glslang::EShTargetSpv, spirv_target_version);
		shader.setEntryPoint("main");

		auto includer = glslang::TShader::ForbidIncluder{};

		auto str = std::string{};
		auto preprocess_result = shader.preprocess(&resources, 460, ECoreProfile, false, true, messages, &str, includer);
		if(!preprocess_result) {
			LogHandler::elog("Renderer"_module, "Failed to preprocess shader, reason: \n{}", shader.getInfoLog());
			LogHandler::dlog("Renderer"_module, "Debug info: \n{}", shader.getInfoDebugLog());
			LogHandler::dlog("Renderer"_module, "Shader: \n{}", c_source);
			return;
		}

		auto parse_result = shader.parse(&resources, 460, ECoreProfile, false, true, messages);
		if(!parse_result) {
			LogHandler::elog("Renderer"_module, "Failed to parse shader, reason: \n{}", shader.getInfoLog());
			LogHandler::dlog("Renderer"_module, "Debug info: \n{}", shader.getInfoDebugLog());
			LogHandler::dlog("Renderer"_module, "Shader: \n{}", c_source);
			return;
		}

		auto program = glslang::TProgram{};
		program.addShader(&shader);

		auto link_result = program.link(messages);
		if(!link_result) {
			LogHandler::elog("Renderer"_module, "Failed to link shader, reason: \n{}.", program.getInfoLog());
			LogHandler::dlog("Renderer"_module, "Debug info: \n{}.", program.getInfoDebugLog());
			LogHandler::dlog("Renderer"_module, "Shader: \n{}", c_source);
			return;
		}

		auto options = glslang::SpvOptions{};
		auto spirv   = std::vector<SpvID>{};

		glslang::GlslangToSpv(*program.getIntermediate(shader_kind), spirv, &options);

		m_source.resize(std::size(spirv) * sizeof(SpvID));
		std::memcpy(std::data(m_source), std::data(spirv), std::size(spirv) * sizeof(SpvID));
	}
}

void Shader::initResources(TBuiltInResource &resources) {
	auto & limits = m_device->physicalDevice().capabilities().limits;

	resources.limits.doWhileLoops = true;
	resources.limits.generalAttributeMatrixVectorIndexing = true;
	resources.limits.generalConstantMatrixVectorIndexing = true;
	resources.limits.generalSamplerIndexing = true;
	resources.limits.generalUniformIndexing = true;
	resources.limits.generalVariableIndexing = true;
	resources.limits.generalVaryingIndexing = true;
	resources.limits.nonInductiveForLoops = true;
	resources.limits.whileLoops = true;
	resources.maxAtomicCounterBindings = 1;
	resources.maxAtomicCounterBufferSize = 16384;
	resources.maxClipDistances = gsl::narrow_cast<int>(limits.max_clip_distances);
	resources.maxClipPlanes = 6;
	resources.maxCombinedAtomicCounterBuffers = 1;
	resources.maxCombinedAtomicCounters = 8;
	resources.maxCombinedClipAndCullDistances = gsl::narrow_cast<int>(limits.max_combined_clip_and_cull_distances);
	resources.maxCombinedImageUniforms = 8;
	resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
	resources.maxCombinedShaderOutputResources = 8;
	resources.maxCombinedTextureImageUnits = 80;
	resources.maxComputeAtomicCounterBuffers = 1;
	resources.maxComputeAtomicCounters = 8;
	resources.maxComputeImageUniforms = 8;
	resources.maxComputeTextureImageUnits = 16;
	resources.maxComputeUniformComponents = 1024;
	resources.maxComputeWorkGroupCountX = gsl::narrow_cast<int>(limits.max_compute_work_group_count[0]);
	resources.maxComputeWorkGroupCountY = gsl::narrow_cast<int>(limits.max_compute_work_group_count[1]);
	resources.maxComputeWorkGroupCountZ = gsl::narrow_cast<int>(limits.max_compute_work_group_count[2]);
	resources.maxComputeWorkGroupSizeX = gsl::narrow_cast<int>(limits.max_compute_work_group_size[0]);
	resources.maxComputeWorkGroupSizeY = gsl::narrow_cast<int>(limits.max_compute_work_group_size[1]);
	resources.maxComputeWorkGroupSizeZ = gsl::narrow_cast<int>(limits.max_compute_work_group_size[2]);
	resources.maxCullDistances = gsl::narrow_cast<int>(limits.max_cull_distances);
	resources.maxDrawBuffers = gsl::narrow_cast<int>(limits.max_color_attachments);
	resources.maxFragmentAtomicCounterBuffers = 1;
	resources.maxFragmentAtomicCounters = 8;
	resources.maxFragmentImageUniforms = 8;
	resources.maxFragmentInputComponents = gsl::narrow_cast<int>(limits.max_fragment_input_components);
	resources.maxFragmentInputVectors = 15;
	resources.maxFragmentUniformComponents = 4096;
	resources.maxFragmentUniformVectors = 16;
	resources.maxGeometryAtomicCounterBuffers = 0;
	resources.maxGeometryAtomicCounters = 0;
	resources.maxGeometryImageUniforms = 0;
	resources.maxGeometryInputComponents = gsl::narrow_cast<int>(limits.max_geometry_input_components);
	resources.maxGeometryOutputComponents = gsl::narrow_cast<int>(limits.max_geometry_output_components);
	resources.maxGeometryOutputVertices = 256;
	resources.maxGeometryTextureImageUnits = 16;
	resources.maxGeometryTotalOutputComponents = gsl::narrow_cast<int>(limits.max_geometry_total_output_components);
	resources.maxGeometryUniformComponents = 1024;
	resources.maxGeometryVaryingComponents = gsl::narrow_cast<int>(limits.max_geometry_output_vertices);
	resources.maxImageSamples = 0;
	resources.maxImageUnits = 8;
	resources.maxLights = 32;
	resources.maxPatchVertices = gsl::narrow_cast<int>(limits.max_tessellation_patch_size );
	resources.maxProgramTexelOffset = gsl::narrow_cast<int>(limits.max_texel_offset );
	resources.maxSamples = 4;
	resources.maxTessControlAtomicCounterBuffers = 0;
	resources.maxTessControlAtomicCounters = 0;
	resources.maxTessControlImageUniforms = 0;
	resources.maxTessControlInputComponents = gsl::narrow_cast<int>(limits.max_tessellation_control_per_vertex_input_components);
	resources.maxTessControlOutputComponents = 128;
	resources.maxTessControlTextureImageUnits = 16;
	resources.maxTessControlTotalOutputComponents = gsl::narrow_cast<int>(limits.max_tessellation_control_total_output_components);
	resources.maxTessControlUniformComponents = 1024;
	resources.maxTessEvaluationAtomicCounterBuffers = 0;
	resources.maxTessEvaluationAtomicCounters = 0;
	resources.maxTessEvaluationImageUniforms = 0;
	resources.maxTessEvaluationInputComponents = gsl::narrow_cast<int>(limits.max_tessellation_evaluation_input_components);
	resources.maxTessEvaluationOutputComponents = gsl::narrow_cast<int>(limits.max_tessellation_evaluation_output_components);
	resources.maxTessEvaluationTextureImageUnits = 16;
	resources.maxTessEvaluationUniformComponents = 1024;
	resources.maxTessGenLevel = gsl::narrow_cast<int>(limits.max_tessellation_generation_level);
	resources.maxTessPatchComponents = gsl::narrow_cast<int>(limits.max_tessellation_control_per_patch_output_components);
	resources.maxTextureCoords = 32;
	resources.maxTextureImageUnits = 32;
	resources.maxTextureUnits = 32;
	resources.maxTransformFeedbackBuffers = 4;
	resources.maxTransformFeedbackInterleavedComponents = 64;
	resources.maxVaryingComponents = 60;
	resources.maxVaryingFloats = 64;
	resources.maxVaryingVectors = 8;
	resources.maxVertexAtomicCounterBuffers = 0;
	resources.maxVertexAtomicCounters = 0;
	resources.maxVertexAttribs = 64;
	resources.maxVertexImageUniforms = 0;
	resources.maxVertexOutputComponents = 64;
	resources.maxVertexOutputVectors = 16;
	resources.maxVertexTextureImageUnits = 32;
	resources.maxVertexUniformComponents = 4096;
	resources.maxVertexUniformVectors = 128;
	resources.maxViewports = gsl::narrow_cast<int>( limits.max_viewports );
	resources.minProgramTexelOffset = -8;
}
