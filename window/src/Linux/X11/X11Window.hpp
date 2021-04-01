// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "../../AbstractWindow.hpp"

/////////// - STL - ///////////
#include <functional>
#include <memory>

/////////// - StormKit::core - ///////////
#include <storm/core/Platform.hpp>

/////////// - XCB - ///////////
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#include <xkbcommon/xkbcommon.h>

namespace storm::window::details {
    class X11InputHandler;
    class STORMKIT_PRIVATE X11Window final: public details::AbstractWindow {
        struct XCBConnectionDeleter {
            void operator()(xcb_connection_t *connection) const noexcept {
                xcb_disconnect(connection);
            }
        };

        using XCBScopedConnection = std::unique_ptr<xcb_connection_t, XCBConnectionDeleter>;

        struct XCBKeySymbolsDeleter {
            void operator()(xcb_key_symbols_t *key_symbols) const noexcept {
                xcb_key_symbols_free(key_symbols);
            }
        };

        using XCBScopedKeySymbols = std::unique_ptr<xcb_key_symbols_t, XCBKeySymbolsDeleter>;

        struct XKBContextDeleter {
            void operator()(xkb_context *context) const noexcept { xkb_context_unref(context); }
        };

        using XKBScopedContext = std::unique_ptr<xkb_context, XKBContextDeleter>;

        struct XKBKeymapDeleter {
            void operator()(xkb_keymap *keymap) const noexcept { xkb_keymap_unref(keymap); }
        };

        using XKBScopedKeymap = std::unique_ptr<xkb_keymap, XKBKeymapDeleter>;

        struct XKBStateDeleter {
            void operator()(xkb_state *state) const noexcept { xkb_state_unref(state); }
        };

        using XKBScopedState = std::unique_ptr<xkb_state, XKBStateDeleter>;

      public:
        struct Handles {
            xcb_connection_t *connection;
            xcb_window_t window;
            xcb_key_symbols_t *key_symbols;
            xkb_state *state;
        };

        X11Window();
        X11Window(std::string title,
                  const VideoSettings &settings,
                  storm::window::WindowStyle style);
        ~X11Window() override;

        void create(std::string title, const VideoSettings &settings, WindowStyle style) override;
        void close() noexcept override;

        [[nodiscard]] bool pollEvent(Event &event) noexcept override;
        [[nodiscard]] bool waitEvent(Event &event) noexcept override;

        void setTitle(std::string title) noexcept override;
        void setVideoSettings(const VideoSettings &settings) noexcept override;

        [[nodiscard]] const core::Extentu &size() const noexcept override;

        [[nodiscard]] bool isOpen() const noexcept override;
        [[nodiscard]] bool isVisible() const noexcept override;

        [[nodiscard]] NativeHandle nativeHandle() const noexcept override;
        [[nodiscard]] const Handles &xcbHandles() const noexcept { return m_handles; }

        [[nodiscard]] static std::vector<VideoSettings> getDesktopModes();
        [[nodiscard]] static VideoSettings getDesktopFullscreenSize();

        ALLOCATE_HELPERS(X11Window)
      private:
        void processEvents(xcb_generic_event_t xevent);
        void updateKeymap();
        void updateXKBMods();

        void registerInputHandler(X11InputHandler &input_handler);
        void unregisterInputHandler(X11InputHandler &input_handler);

        XCBScopedConnection m_connection;
        xcb_window_t m_window;
        XCBScopedKeySymbols m_key_symbols;

        bool m_has_xkb = false;
        XKBScopedContext m_xkb_context;
        XKBScopedKeymap m_xkb_keymap;
        XKBScopedState m_xkb_state;

        struct XKBMods {
            xkb_mod_index_t shift;
            xkb_mod_index_t lock;
            xkb_mod_index_t control;
            xkb_mod_index_t mod1;
            xkb_mod_index_t mod2;
            xkb_mod_index_t mod3;
            xkb_mod_index_t mod4;
            xkb_mod_index_t mod5;
        } m_xkb_mods;

        xcb_intern_atom_cookie_t m_protocol_cookie;
        xcb_intern_atom_reply_t *m_protocol_reply;
        xcb_intern_atom_cookie_t m_close_cookie;
        xcb_intern_atom_reply_t *m_close_reply;

        std::vector<X11InputHandler *> m_input_handlers;

        core::Extentu m_extent;

        bool m_is_open;
        bool m_is_visible;

        Handles m_handles;

        mutable core::Int16 m_mouse_x;
        mutable core::Int16 m_mouse_y;

        friend class X11InputHandler;
    };

    DECLARE_PTR_AND_REF(X11Window)
} // namespace storm::window::details
