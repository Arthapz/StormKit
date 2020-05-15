#pragma once

#include <storm/core/Platform.hpp>
#include <storm/window/AbstractWindow.hpp>

#ifdef __OBJC__
@class StormWindowController;
using StormWindowControllerOwnedPtr = StormWindowController *;
#else
using StormWindowControllerOwnedPtr = void *;
#endif

class ContextImpl;
class WindowImpl: public storm::window::AbstractWindow {
  public:
    WindowImpl() noexcept;
    WindowImpl(const std::string &title,
               const storm::window::VideoSettings &settings,
               storm::window::WindowStyle style) noexcept;
    ~WindowImpl() override;

    void create(const std::string &title,
                const storm::window::VideoSettings &settings,
                storm::window::WindowStyle style) noexcept override;
    void close() noexcept override;

    bool pollEvent(storm::window::Event &event, void *native_event) noexcept override;
    bool waitEvent(storm::window::Event &event,
                   [[maybe_unused]] void *native_event) noexcept override;

    void setTitle(const std::string &title) noexcept override;
    void setVideoSettings(const storm::window::VideoSettings &settings) noexcept override;

    storm::core::Extentu size() const noexcept override;

    bool isOpen() const noexcept override;
    bool isVisible() const noexcept override;

    // ContextImpl &context() noexcept override { return *m_context; }

    storm::window::NativeHandle nativeHandle() const noexcept override;

  private:
    void initCocoaProcess();

    StormWindowControllerOwnedPtr m_controller;
};
