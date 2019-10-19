#pragma once

#include <SFML/Window/Window.hpp>
#include <storm/core/Platform.hpp>
#include <storm/window/AbstractWindow.hpp>

namespace storm::window {
	class WindowImpl : public AbstractWindow {
	  public:
		WindowImpl();
		WindowImpl(
			const std::string &title,
			const storm::window::VideoSettings &settings,
			storm::window::WindowStyle style,
			const storm::window::ContextSettings &context_settings) noexcept;
		~WindowImpl() override;

		void create(const std::string &title,
					const storm::window::VideoSettings &settings,
					storm::window::WindowStyle style,
					const storm::window::ContextSettings
						&context_settings) noexcept override;
		void close() noexcept override;
		void display() noexcept override;

		// BLC
		bool pollEvent(storm::window::Event &event) noexcept override;
		void waitEvent() noexcept override;

		void setTitle(const std::string &title) noexcept override;
		void setVideoSettings(
			const storm::window::VideoSettings &settings) noexcept override;

		VideoSettings::Size size() const noexcept override;

		bool isOpen() const noexcept override;
		bool isVisible() const noexcept override;

		void setMousePosition(std::uint32_t x, std::uint32_t y) const
			noexcept override;

		storm::window::NativeHandle nativeHandle() const noexcept override;

	  private:
		sf::Window m_window;
	};
} // namespace storm::window
