// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Flags.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/AbstractWindow.hpp>
#include <storm/window/Fwd.hpp>

/////////// - StormKit::module - ///////////
#include <storm/module/Fwd.hpp>

namespace storm::window {
    class STORM_PUBLIC Window: public core::NonCopyable {
      public:
        enum class WM { Win32, X11, Wayland, macOS, iOS, Android, Switch };

        Window() noexcept;
        Window(const std::string &title, const VideoSettings &settings, WindowStyle style) noexcept;
        ~Window();

        Window(Window &&);
        Window &operator=(Window &&);

        inline void create(const std::string &title,
                           const VideoSettings &settings,
                           WindowStyle style) noexcept;
        inline void close() noexcept;

        inline bool pollEvent(Event &event, void *native_event = nullptr) noexcept;
        inline bool waitEvent(Event &event, void *native_event = nullptr) noexcept;

        inline void setTitle(const std::string &title) noexcept;
        inline void setVideoSettings(const VideoSettings &settings) noexcept;

        inline core::Extentu size() const noexcept;
        inline const std::string &title() const noexcept;
        inline VideoSettings videoSettings() const noexcept;

        inline bool isOpen() const noexcept;
        inline bool isVisible() const noexcept;

        inline NativeHandle nativeHandle() const noexcept;

        static core::span<const VideoSettings> getDesktopModes();
        static const VideoSettings &getDesktopFullscreenSize();

      private:
        static void initPlugin();

        static WM detectWM() noexcept;

        AbstractWindowObserverPtr m_impl;
    };
} // namespace storm::window

#define DEFINE_WINDOW_PLUGIN(WINDOW_CLASS_NAME, INPUT_HANDLER_CLASS_NAME)                          \
    static auto windows        = std::vector<std::unique_ptr<WINDOW_CLASS_NAME>> {};               \
    static auto input_handlers = std::vector<std::unique_ptr<INPUT_HANDLER_CLASS_NAME>> {};        \
                                                                                                   \
    extern "C" {                                                                                   \
    STORM_EXPORT storm::window::AbstractWindow *createWindow();                                    \
    STORM_EXPORT void destroyWindow(storm::window::AbstractWindow *);                              \
    STORM_EXPORT storm::window::AbstractInputHandler *                                             \
        createInputHandler(const storm::window::Window &window);                                   \
    STORM_EXPORT void destroyInputHandler(storm::window::AbstractInputHandler *);                  \
    STORM_EXPORT const storm::window::VideoSettings *                                              \
        getDesktopModes(storm::core::ArraySize &size);                                             \
    STORM_EXPORT const storm::window::VideoSettings *getDesktopFullscreenSize();                   \
    }                                                                                              \
                                                                                                   \
    storm::window::AbstractWindow *createWindow() {                                                \
        auto &window = windows.emplace_back(std::make_unique<WINDOW_CLASS_NAME>());                \
                                                                                                   \
        return window.get();                                                                       \
    }                                                                                              \
                                                                                                   \
    void destroyWindow(storm::window::AbstractWindow *window) {                                    \
        if (window == nullptr) return;                                                             \
        const auto it = std::find_if(std::begin(windows),                                          \
                                     std::end(windows),                                            \
                                     [&window](const auto &win) { return win.get() == window; });  \
                                                                                                   \
        if (it != std::end(windows)) windows.erase(it);                                            \
    }                                                                                              \
                                                                                                   \
    storm::window::AbstractInputHandler *createInputHandler(const storm::window::Window &window) { \
        auto &input_handler =                                                                      \
            input_handlers.emplace_back(std::make_unique<INPUT_HANDLER_CLASS_NAME>(window));       \
                                                                                                   \
        return input_handler.get();                                                                \
    }                                                                                              \
                                                                                                   \
    void destroyInputHandler(storm::window::AbstractInputHandler *handler) {                       \
        if (handler == nullptr) return;                                                            \
        const auto it = std::find_if(std::begin(input_handlers),                                   \
                                     std::end(input_handlers),                                     \
                                     [&handler](const auto &ih) { return ih.get() == handler; });  \
                                                                                                   \
        if (it != std::end(input_handlers)) input_handlers.erase(it);                              \
    }

#include "Window.inl"
