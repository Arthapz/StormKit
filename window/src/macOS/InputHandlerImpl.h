// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Math.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/window/Fwd.hpp>
#include <storm/window/Key.hpp>

#include <Carbon/Carbon.h>
#include <IOKit/hid/IOHIDDevice.h>
#include <IOKit/hid/IOHIDManager.h>

namespace storm::window {
	class STORM_PRIVATE InputHandlerImpl: public core::NonCopyable {
	  public:
		InputHandlerImpl();
		~InputHandlerImpl();

		InputHandlerImpl(InputHandlerImpl &&);
		InputHandlerImpl &operator=(InputHandlerImpl &&);

		static bool isKeyPressed(Key key);
		static bool isMouseButtonPressed(MouseButton button);
		static void setMousePosition(core::Position2u position);
		static void setMousePosition(core::Position2i position, const Window &relative_to);
		static core::Position2u getMousePosition();
		static core::Position2i getMousePosition(const Window &relative_to);

		static void setVirtualKeyboardVisible(bool visible);

	  private:
		static void initIOHID();
		static void initializeKeyboard();
		static void loadKeyboard(IOHIDDeviceRef keyboard);
		static void loadKey(IOHIDElementRef key);
		static CFDictionaryRef copyDevicesMask(core::UInt32 page, core::UInt32 usage);
		static CFSetRef copyDevices(core::UInt32 page, core::UInt32 usage);

		using IOHIDElements = std::vector<IOHIDElementRef>;

		static inline CFDataRef m_layout_data	 = nullptr;
		static inline UCKeyboardLayout *m_layout = nullptr;
		static inline IOHIDManagerRef m_manager	 = nullptr;

		static inline IOHIDElements m_keys[static_cast<core::ArraySize>(Key::KEYCOUNT)] = {};
	};
} // namespace storm::window
