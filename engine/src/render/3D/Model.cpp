#include <storm/engine/render/3D/Model.hpp>

using namespace storm;
using namespace storm::engine;

/////////////////////////////////////
/////////////////////////////////////
Model::Model(Engine &engine) : m_engine { engine } {
}

/////////////////////////////////////
/////////////////////////////////////
Model::Model(Model &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
auto Model::operator=(Model &&) noexcept -> Model & = default;

/////////////////////////////////////
/////////////////////////////////////
void Model::load(std::filesystem::path path) {
}
