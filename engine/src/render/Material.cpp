/////////// - StormKit::engine - ///////////
#include <storm/engine/render/Material.hpp>

using namespace storm;
using namespace storm::engine;


////////////////////////////////////////
////////////////////////////////////////
Material::Material() noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
Material::~Material() = default;

////////////////////////////////////////
////////////////////////////////////////
Material::Material(Material &&) noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
auto Material::operator=(Material &&) noexcept -> Material& = default;