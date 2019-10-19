#include <storm/graphics/framegraph/FrameGraphResources.hpp>

#include <storm/render/core/Device.hpp>
#include <storm/render/resource/HardwareBuffer.hpp>
#include <storm/render/resource/Texture.hpp>

#include <gsl/gsl_assert>

using namespace storm;
using namespace storm::graphics;

////////////////////////////////////////
////////////////////////////////////////
FrameGraphTextureResource::FrameGraphTextureResource(
	std::string &&name, FrameGraphTextureDesc &&description,
	render::DeviceCRef device)
	: m_device{device}, m_name{std::move(name)},
	  m_description{std::move(description)}, m_texture{nullptr} {
}

////////////////////////////////////////
////////////////////////////////////////
FrameGraphTextureResource::FrameGraphTextureResource(
	std::string &&name, FrameGraphTextureDesc &&description,
	render::Texture &texture, render::DeviceCRef device)
	: m_device{device}, m_name{std::move(name)},
	  m_description{std::move(description)}, m_texture{texture} {
}

////////////////////////////////////////
////////////////////////////////////////
FrameGraphTextureResource::~FrameGraphTextureResource() = default;

////////////////////////////////////////
////////////////////////////////////////
FrameGraphTextureResource::FrameGraphTextureResource(
	const FrameGraphTextureResource &copy)
	: m_device{copy.m_device}, m_name{copy.m_name}, m_description{
														copy.m_description} {

	if (std::holds_alternative<render::TextureRef>(copy.m_texture))
		m_texture = std::get<render::TextureRef>(copy.m_texture);
	else
		m_texture = nullptr;

	m_creator = copy.m_creator;
	m_readers = copy.m_readers;
	m_writers = copy.m_writers;
}

////////////////////////////////////////
////////////////////////////////////////
FrameGraphTextureResource &FrameGraphTextureResource::operator=(
    const FrameGraphTextureResource &copy) {
	if (&copy == this)
		return *this;

	m_device	  = copy.m_device;
	m_name		  = copy.m_name;
	m_description = copy.m_description;

	if (std::holds_alternative<render::TextureRef>(copy.m_texture))
		m_texture = std::get<render::TextureRef>(copy.m_texture);
	else
		m_texture = nullptr;

	m_creator = copy.m_creator;
	m_readers = copy.m_readers;
	m_writers = copy.m_writers;

	return *this;
};

////////////////////////////////////////
////////////////////////////////////////
FrameGraphTextureResource::FrameGraphTextureResource(
	FrameGraphTextureResource &&) = default;

////////////////////////////////////////
////////////////////////////////////////
FrameGraphTextureResource &FrameGraphTextureResource::operator=(
    FrameGraphTextureResource &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void FrameGraphTextureResource::realize() {
	STORM_EXPECTS(isTransient());

	auto image_usage = render::ImageUsage::Color_Attachment |
                       render::ImageUsage::Transfert_Src |
                       render::ImageUsage::Sampled;

	if (render::isDepthFormat(m_description.format))
		image_usage = render::ImageUsage::Depth_Stencil_Attachment;

	auto texture = m_device.get().createTexture();
	texture->createTextureData(m_description.extent, m_description.format,
							   m_description.samples, m_description.levels,
							   m_description.layers, image_usage);

	m_texture = std::move(texture);
}

////////////////////////////////////////
////////////////////////////////////////
void FrameGraphTextureResource::derealize() {
	STORM_EXPECTS(isTransient());

	std::get<render::TextureOwnedPtr>(m_texture).reset(nullptr);
}

////////////////////////////////////////
////////////////////////////////////////
FrameGraphResources::FrameGraphResources(render::DeviceCRef device)
	: m_device{device} {
}

////////////////////////////////////////
////////////////////////////////////////
FrameGraphResources::~FrameGraphResources() = default;

////////////////////////////////////////
////////////////////////////////////////
FrameGraphResources::FrameGraphResources(const FrameGraphResources &copy)
	: m_device{copy.m_device} {
	for (const auto &texture : copy.m_textures)
		m_textures.emplace_back(
			std::make_unique<FrameGraphTextureResource>(*texture));
}

////////////////////////////////////////
////////////////////////////////////////
FrameGraphResources &
    FrameGraphResources::operator=(const FrameGraphResources &copy) {
	if (&copy == this)
		return *this;

	m_device = copy.m_device;

	for (const auto &texture : copy.m_textures)
		m_textures.emplace_back(
			std::make_unique<FrameGraphTextureResource>(*texture));

	return *this;
}

////////////////////////////////////////
////////////////////////////////////////
FrameGraphResources::FrameGraphResources(FrameGraphResources &&) = default;

////////////////////////////////////////
////////////////////////////////////////
FrameGraphResources &
    FrameGraphResources::operator=(FrameGraphResources &&) = default;

////////////////////////////////////////
////////////////////////////////////////
FrameGraphTextureResource &
	FrameGraphResources::setRetainedTexture(std::string &&name,
											FrameGraphTextureDesc &&desc,
											render::Texture &texture) {
	auto it =
		std::find_if(std::begin(m_textures), std::end(m_textures),
					 [&name](const auto &ptr) { return ptr->name() == name; });

	auto resource = std::make_unique<FrameGraphTextureResource>(
		std::move(name), std::move(desc), texture, m_device);

	if (it != std::end(m_textures)) {
		*it = std::move(resource);

		return *(*it);
	}

	return *m_textures.emplace_back(std::move(resource));
}

////////////////////////////////////////
////////////////////////////////////////
void FrameGraphResources::setRetainedBuffer(std::string &&name,
											FrameGraphBufferDesc &&desc,
											render::HardwareBuffer &buffer) {
}

FrameGraphTextureResource &
	FrameGraphResources::addTransientTexture(std::string &&name,
											 FrameGraphTextureDesc &&desc) {
	const auto it =
		std::find_if(std::cbegin(m_textures), std::cend(m_textures),
					 [&name](const auto &ptr) { return ptr->name() == name; });

	STORM_EXPECTS(it == std::cend(m_textures));

	return *m_textures.emplace_back(std::make_unique<FrameGraphTextureResource>(
		std::move(name), std::move(desc), m_device));
}
