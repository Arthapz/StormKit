#include <storm/engine/framegraph/FramePassResource.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
FramePassBaseResource::FramePassBaseResource(std::string name, bool transient) noexcept
    : m_is_transient { transient }, m_name { name } {
}

////////////////////////////////////////
////////////////////////////////////////
FramePassBaseResource::~FramePassBaseResource() = default;

////////////////////////////////////////
////////////////////////////////////////
FramePassBaseResource::FramePassBaseResource(FramePassBaseResource &&) = default;

////////////////////////////////////////
////////////////////////////////////////
FramePassBaseResource &FramePassBaseResource::operator=(FramePassBaseResource &&) = default;
