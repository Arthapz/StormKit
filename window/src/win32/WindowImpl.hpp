#pragma once

#include <storm/core/Platform.hpp>
#include <storm/window/AbstractWindow.hpp>

namespace storm::window {
    class WindowImpl: public AbstractWindow {
      public:
        WindowImpl();
        WindowImpl(const std::string &title,
                   const storm::window::VideoSettings &settings,
                   storm::window::WindowStyle style);
        ~WindowImpl() override;

        void create(const std::string &title,
                    const storm::window::VideoSettings &settings,
                    storm::window::WindowStyle style) override;
        void close() noexcept override;
        void display() noexcept override;

        // BLC
        bool pollEvent(storm::window::Event &event, void *native_event) noexcept override;
        bool waitEvent(Event &event, void *native_event) noexcept override;

        void setTitle(const std::string &title) noexcept override;
        void setVideoSettings(const storm::window::VideoSettings &settings) noexcept override;

        core::Extentu size() const noexcept override;

        bool isOpen() const noexcept override;
        bool isVisible() const noexcept override;

        storm::window::NativeHandle nativeHandle() const noexcept override;

      private:
        static inline constexpr const auto CLASS_NAME = L"Stormkit_Window";
        static LRESULT CALLBACK globalOnEvent(HWND handle,
                                              UINT message,
                                              WPARAM w_param,
                                              LPARAM l_param);

        void registerWindowClass();
        void processEvents(UINT message, WPARAM w_param, LPARAM l_param);

        bool m_is_open       = false;
        bool m_is_visible    = false;
        bool m_is_fullscreen = false;
        bool m_resizing      = false;
        bool m_mouse_inside  = false;

        core::Extentu m_last_size = { 0u, 0u };

        HWND m_window_handle = nullptr;
    };
} // namespace storm::window
