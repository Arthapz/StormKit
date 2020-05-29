#include <storm/engine/framegraph/FramePassResource.hpp>

#include <storm/render/core/Device.hpp>

#include <storm/render/resource/HardwareBuffer.hpp>
#include <storm/render/resource/Texture.hpp>

#include <storm/core/Assert.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
void FrameGraphTexture::realize(const render::Device &device, const Descriptor &descriptor) {
    auto texture = device.createTexturePtr(descriptor.type);
    texture->createTextureData(descriptor.extent,
                               descriptor.format,
                               { .samples    = descriptor.samples,
                                 .mip_levels = descriptor.levels,
                                 .layers     = 1u,
                                 .usage      = descriptor.usage });

    storage = std::move(texture);
}

////////////////////////////////////////
////////////////////////////////////////
void FrameGraphTexture::derealize() {
    auto &_texture = std::get<render::TextureOwnedPtr>(storage);
    _texture.reset();
}

////////////////////////////////////////
////////////////////////////////////////
void FrameGraphBuffer::realize(const render::Device &device, const Descriptor &descriptor) {
    ASSERT(true, "Not yet implemented");
}

////////////////////////////////////////
////////////////////////////////////////
void FrameGraphBuffer::derealize() {
    ASSERT(true, "Not yet implemented");
}

////////////////////////////////////////
////////////////////////////////////////
FramePassResourceBase::FramePassResourceBase(std::string name,
                                             core::UInt32 id,
                                             bool transient) noexcept
    : m_name { std::move(name) }, m_id { id }, m_is_transient { transient } {
}

////////////////////////////////////////
////////////////////////////////////////
FramePassResourceBase::~FramePassResourceBase() = default;

////////////////////////////////////////
////////////////////////////////////////
FramePassResourceBase::FramePassResourceBase(FramePassResourceBase &&) = default;

////////////////////////////////////////
////////////////////////////////////////
FramePassResourceBase &FramePassResourceBase::operator=(FramePassResourceBase &&) = default;
