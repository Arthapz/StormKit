// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Math.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Pimpl.hpp>
#include <storm/core/Platform.hpp>

#include <storm/window/ForwardDeclarations.hpp>

namespace storm::window {
	class InputHandlerImpl;
	class STORM_PUBLIC InputHandler : public core::NonCopyable {
	  public:
		using Callback = std::function<void()>;
		InputHandler();
		~InputHandler();

		InputHandler(InputHandler &&);
		InputHandler &operator=(InputHandler &&);

		static bool isKeyPressed(Key key);
		static bool isMouseButtonPressed(MouseButton button);
		static void setMousePosition(core::Position2u position);
		static void setMousePosition(core::Position2u position,
									 const Window &relative_to);
		static core::Position2u getMousePosition();
		static core::Position2u getMousePosition(const Window &relative_to);

		static void setVirtualKeyboardVisible(bool visible);

		// TODO implement a input handler like gainput
		// TODO implement touch events
	  private:
		core::Pimpl<InputHandlerImpl> m_impl;
	};
} // namespace storm::window
