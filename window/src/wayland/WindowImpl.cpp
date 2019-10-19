#include "WindowImpl.hpp"

using namespace storm::window;

WindowImpl::WindowImpl()  = default;
WindowImpl::~WindowImpl() = default;

WindowImpl::WindowImpl(const std::string &title, const VideoSettings &settings,
					   WindowStyle style,
					   const ContextSettings &context_settings) noexcept {
	create(title, settings, style, context_settings);
}

void WindowImpl::create(const std::string &title, const VideoSettings &settings,
						WindowStyle style,
						const ContextSettings &context_settings) noexcept {
	auto _style = static_cast<sf::Uint32>(sf::Style::Titlebar);
	if ((style & WindowStyle::Close) == WindowStyle::Close)
		_style |= static_cast<sf::Uint32>(sf::Style::Close);

	if ((style & WindowStyle::Resizable) == WindowStyle::Resizable)
		_style |= static_cast<sf::Uint32>(sf::Style::Resize);

	if ((style & WindowStyle::Fullscreen) == WindowStyle::Fullscreen)
		_style |= static_cast<sf::Uint32>(sf::Style::Fullscreen);

	auto _settings			 = sf::ContextSettings{};
	_settings.attributeFlags = sf::ContextSettings::Core;
	_settings.majorVersion   = context_settings.major_version;
	_settings.minorVersion   = context_settings.minor_version;

	m_window.create({settings.size.w, settings.size.h, settings.bpp}, title,
					_style, _settings);
}

void WindowImpl::close() noexcept {
	m_window.close();
}

void WindowImpl::display() noexcept {
	m_window.display();
}

bool WindowImpl::pollEvent(Event &event) noexcept {
	return false;
}

void WindowImpl::waitEvent() noexcept {
}

void WindowImpl::setTitle(const std::string &title) noexcept {
	m_window.setTitle(title);
}

void WindowImpl::setVideoSettings(
	const storm::window::VideoSettings &settings) noexcept {
}

VideoSettings::Size WindowImpl::size() const noexcept {
	const auto size = m_window.getSize();
	return {size.x, size.y};
}

bool WindowImpl::isOpen() const noexcept {
	return m_window.isOpen();
}

bool WindowImpl::isVisible() const noexcept {
	return true;
}

void WindowImpl::setMousePosition(uint32_t x, uint32_t y) const noexcept {
}

storm::window::NativeHandle WindowImpl::nativeHandle() const noexcept {
	return reinterpret_cast<void *>(m_window.getSystemHandle());
}
