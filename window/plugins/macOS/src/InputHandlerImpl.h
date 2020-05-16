// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Math.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/AbstractInputHandler.hpp>
#include <storm/window/Fwd.hpp>
#include <storm/window/Key.hpp>

/////////// - Cocoa - ///////////
#include <Carbon/Carbon.h>
#include <IOKit/hid/IOHIDDevice.h>
#include <IOKit/hid/IOHIDManager.h>

namespace storm::window {
    class InputHandlerImpl: public AbstractInputHandler {
      public:
        explicit InputHandlerImpl(const Window &window);
        ~InputHandlerImpl() override;

        InputHandlerImpl(InputHandlerImpl &&);
        InputHandlerImpl &operator=(InputHandlerImpl &&);

        bool isKeyPressed(Key key) const noexcept override;
        bool isMouseButtonPressed(MouseButton button) const noexcept override;

        core::Position2u getMousePositionOnDesktop() const noexcept override;
        void setMousePositionOnDesktop(core::Position2u position) noexcept override;

        core::Position2i getMousePositionOnWindow() const noexcept override;
        void setMousePositionOnWindow(core::Position2i position) noexcept override;

        void setVirtualKeyboardVisible(bool visible) noexcept override;

      private:
        static void initIOHID();
        static void initializeKeyboard();
        static void loadKeyboard(IOHIDDeviceRef keyboard);
        static void loadKey(IOHIDElementRef key);
        static CFDictionaryRef copyDevicesMask(core::UInt32 page, core::UInt32 usage);
        static CFSetRef copyDevices(core::UInt32 page, core::UInt32 usage);

        using IOHIDElements = std::vector<IOHIDElementRef>;

        static inline CFDataRef m_layout_data    = nullptr;
        static inline UCKeyboardLayout *m_layout = nullptr;
        static inline IOHIDManagerRef m_manager  = nullptr;

        static inline IOHIDElements m_keys[static_cast<core::ArraySize>(Key::Keycount)] = {};
    };
} // namespace storm::window
