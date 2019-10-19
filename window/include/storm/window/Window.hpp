// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>

#ifdef STORM_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

#include <storm/core/Flags.hpp>
#include <storm/window/AbstractWindow.hpp>

namespace storm::window {
	class STORM_PUBLIC STORM_EBCO Window : public core::NonCopyable {
	  public:
		Window() noexcept;
		Window(const std::string &title, const VideoSettings &settings,
			   WindowStyle style) noexcept;
		~Window();

		Window(Window &&);
		Window &operator=(Window &&);

		inline void create(const std::string &title,
						   const VideoSettings &settings,
						   WindowStyle style) noexcept {
			m_impl->create(title, settings, style);
		}
		inline void close() noexcept {
			m_impl->close();
		}
		void display() noexcept;

		inline bool pollEvent(Event &event,
							  void *native_event = nullptr) noexcept {
			return m_impl->pollEvent(event, native_event);
		}
		inline bool waitEvent(Event &event,
							  void *native_event = nullptr) noexcept {
			return m_impl->waitEvent(event, native_event);
		}

		inline void setTitle(const std::string &title) noexcept {
			m_impl->setTitle(title);
		}
		inline void setVideoSettings(const VideoSettings &settings) noexcept {
			m_impl->setVideoSettings(settings);
		}

		inline core::Extent size() const noexcept {
			return m_impl->size();
		}

		inline const std::string &title() const noexcept {
			return m_impl->title();
		}
		inline VideoSettings videoSettings() const noexcept {
			return m_impl->videoSettings();
		}
		inline bool isOpen() const noexcept {
			return m_impl->isOpen();
		}
		inline bool isVisible() const noexcept {
			return m_impl->isVisible();
		}

		inline NativeHandle nativeHandle() const noexcept {
			return m_impl->nativeHandle();
		}

	  private:
		AbstractWindowOwnedPtr m_impl;
	};
} // namespace storm::window

#ifdef STORM_COMPILER_MSVC
#pragma warning(pop)
#endif
