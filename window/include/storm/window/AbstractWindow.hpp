// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <queue>
#include <string>

#include <storm/core/Math.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#ifdef STORM_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

#include <storm/window/Event.hpp>
#include <storm/window/ForwardDeclarations.hpp>
#include <storm/window/VideoSettings.hpp>
#include <storm/window/WindowStyle.hpp>

namespace storm::window {
	class STORM_PUBLIC STORM_EBCO AbstractWindow : public core::NonCopyable {
	  public:
		AbstractWindow();
		virtual ~AbstractWindow();

		virtual void create(const std::string &title,
							const VideoSettings &settings,
							WindowStyle style) = 0;
		virtual void close() noexcept		   = 0;
		virtual void display() noexcept		   = 0;

		// BLC
		virtual bool pollEvent(Event &event,
							   void *native_event = nullptr) noexcept;
		virtual bool waitEvent(Event &event,
							   void *native_event = nullptr) noexcept = 0;

		virtual void setTitle(const std::string &title) noexcept = 0;
		virtual void
			setVideoSettings(const VideoSettings &settings) noexcept = 0;

		virtual core::Extent size() const noexcept = 0;

		inline const std::string &title() const noexcept {
			return m_title;
		}
		inline const VideoSettings &videoSettings() const noexcept {
			return m_video_settings;
		}
		virtual bool isOpen() const noexcept	= 0;
		virtual bool isVisible() const noexcept = 0;

		virtual NativeHandle nativeHandle() const noexcept = 0;

		void mouseDownEvent(MouseButton button, std::int16_t x,
							std::int16_t y) noexcept;
		void mouseUpEvent(MouseButton button, std::int16_t x,
						  std::int16_t y) noexcept;

		void mouseMoveEvent(std::int16_t x, std::int16_t y) noexcept;

		void mouseEnteredEvent() noexcept;
		void mouseExitedEvent() noexcept;

		void keyDownEvent(Key key) noexcept;
		void keyUpEvent(Key key) noexcept;

		void closeEvent() noexcept;

		void resizeEvent(std::uint16_t width, std::uint16_t height) noexcept;
		void minimizeEvent() noexcept;
		void maximizeEvent() noexcept;

	  protected:
		void pushEvent(Event event) noexcept;

		std::string m_title;
		VideoSettings m_video_settings;
		WindowStyle m_style;

		std::queue<Event> m_events;
	};
} // namespace storm::window

#ifdef STORM_COMPILER_MSVC
#pragma warning(pop)
#endif
