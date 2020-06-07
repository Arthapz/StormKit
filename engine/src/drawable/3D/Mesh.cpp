/////////// - StormKit::render - ///////////
#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/core/Surface.hpp>

#include <storm/render/sync/Fence.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/drawable/Mesh.hpp>
#include <storm/engine/drawable/SubMesh.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
Mesh::Mesh(Engine &engine, const Material &material, std::string name)
    : Drawable { engine }, m_name { std::move(name) }, m_material { &material } {
    auto &device = m_engine->device();
}

////////////////////////////////////////
////////////////////////////////////////
Mesh::~Mesh() = default;

////////////////////////////////////////
////////////////////////////////////////
Mesh::Mesh(Mesh &&) = default;

////////////////////////////////////////
////////////////////////////////////////
Mesh &Mesh::operator=(Mesh &&) = default;
