#include <storm/engine/framegraph/FramePass.hpp>

#include <storm/render/resource/Texture.hpp>

using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
FramePassBase::FramePassBase(std::string name, core::UInt32 id, PassType type) noexcept
    : m_name { std::move(name) }, m_id { id }, m_type { type } {
}

////////////////////////////////////////
////////////////////////////////////////
FramePassBase::~FramePassBase() = default;

////////////////////////////////////////
////////////////////////////////////////
FramePassBase::FramePassBase(FramePassBase &&drawable) = default;

////////////////////////////////////////
////////////////////////////////////////
FramePassBase &FramePassBase::operator=(FramePassBase &&drawable) = default;
