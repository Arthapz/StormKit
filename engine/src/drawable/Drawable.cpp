/////////// - StormKit::render - ///////////
#include <storm/render/resource/Texture.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/drawable/Drawable.hpp>

#include <storm/engine/material/Material.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
Drawable::Drawable(Engine &engine) : m_engine { &engine } {
}

////////////////////////////////////////
////////////////////////////////////////
Drawable::~Drawable() = default;

////////////////////////////////////////
////////////////////////////////////////
Drawable::Drawable(Drawable &&) = default;

////////////////////////////////////////
////////////////////////////////////////
Drawable &Drawable::operator=(Drawable &&) = default;
