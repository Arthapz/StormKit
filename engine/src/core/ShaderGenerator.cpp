/////////// - StormKit::engine - ///////////
#include <storm/engine/core/ShaderGenerator.hpp>

#include <storm/engine/mesh/StaticMesh.hpp>

#include <storm/engine/material/Material.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/resource/Shader.hpp>

#undef CASE

/////////// - ShaderWriter - ///////////
#include <ShaderWriter/Writer.hpp>

using namespace storm;
using namespace storm::engine;

namespace storm::engine {
    struct ShaderGeneratorPrivate {};
} // namespace storm::engine

/////////////////////////////////////
/////////////////////////////////////
ShaderGenerator::ShaderGenerator(const Engine &engine) : m_engine { &engine } {
}

/////////////////////////////////////
/////////////////////////////////////
ShaderGenerator::~ShaderGenerator() = default;

/////////////////////////////////////
/////////////////////////////////////
ShaderGenerator::ShaderGenerator(ShaderGenerator &&) = default;

/////////////////////////////////////
/////////////////////////////////////
ShaderGenerator &ShaderGenerator::operator=(ShaderGenerator &&) = default;

/////////////////////////////////////
/////////////////////////////////////
ShaderGenerator::Shaders
    ShaderGenerator::generateShaders(const render::GraphicsPipelineState &pipeline) const {
    auto shaders = Shaders {};

    auto vertex_writer = sdw::VertexWriter {};

    return shaders;
}
