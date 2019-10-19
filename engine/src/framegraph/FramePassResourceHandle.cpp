#include <storm/engine/framegraph/FramePassResourceHandle.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
FramePassResourceHandle::FramePassResourceHandle() noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
FramePassResourceHandle::FramePassResourceHandle(core::UInt32 index) noexcept : m_index { index } {
}

////////////////////////////////////////
////////////////////////////////////////
FramePassResourceHandle::FramePassResourceHandle(const FramePassResourceHandle &) = default;

////////////////////////////////////////
////////////////////////////////////////
FramePassResourceHandle &
    FramePassResourceHandle::operator=(const FramePassResourceHandle &) = default;

////////////////////////////////////////
////////////////////////////////////////
FramePassResourceHandle::FramePassResourceHandle(FramePassResourceHandle &&) = default;

////////////////////////////////////////
////////////////////////////////////////
FramePassResourceHandle &FramePassResourceHandle::operator=(FramePassResourceHandle &&) = default;

////////////////////////////////////////
////////////////////////////////////////
FramePassResourceHandle::~FramePassResourceHandle() = default;

/////////////////////////////////////
/////////////////////////////////////
namespace std {
    core::Hash64
        hash<FramePassResourceHandle>::operator()(const FramePassResourceHandle &handle) const
        noexcept {
        return handle.index();
    }
} // namespace std
