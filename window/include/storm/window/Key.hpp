// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Strings.hpp>

#undef DELETE

namespace storm::window {
	enum class Key : std::int8_t {
		UNKNOW = -1,
		A	  = 0,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		NUM0,
		NUM1,
		NUM2,
		NUM3,
		NUM4,
		NUM5,
		NUM6,
		NUM7,
		NUM8,
		NUM9,
		ESCAPE,
		LCONTROL,
		LSHIFT,
		LALT,
		LSYSTEM,
		RCONTROL,
		RSHIFT,
		RALT,
		RSYSTEM,
		MENU,
		LBRACKET,
		RBRACKET,
		SEMICOLON,
		COMMA,
		PERIOD,
		QUOTE,
		SLASH,
		BACKSLASH,
		TILDE,
		EQUAL,
		HYPHEN,
		SPACE,
		ENTER,
		BACKSPACE,
		TAB,
		PAGEUP,
		PAGEDOWN,
		BEGIN,
		END,
		HOME,
		INSERT,
		DELETE,
		ADD,
		SUBSTRACT,
		MULTIPLY,
		DIVIDE,
		LEFT,
		RIGHT,
		UP,
		DOWN,
		NUMPAD0,
		NUMPAD1,
		NUMPAD2,
		NUMPAD3,
		NUMPAD4,
		NUMPAD5,
		NUMPAD6,
		NUMPAD7,
		NUMPAD8,
		NUMPAD9,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		F13,
		F14,
		F15,
		PAUSE,
		KEYCOUNT,
	};

	////////////////////////////////////////
	////////////////////////////////////////
	inline std::string keyToString(storm::window::Key value) {
		using namespace storm::window;

		switch (value) {
		case Key::UNKNOW:
			return "UNKNOW";
		case Key::A:
			return "A";
		case Key::B:
			return "B";
		case Key::C:
			return "C";
		case Key::D:
			return "D";
		case Key::E:
			return "E";
		case Key::F:
			return "F";
		case Key::G:
			return "G";
		case Key::H:
			return "H";
		case Key::I:
			return "I";
		case Key::J:
			return "J";
		case Key::K:
			return "K";
		case Key::L:
			return "L";
		case Key::M:
			return "M";
		case Key::N:
			return "N";
		case Key::O:
			return "O";
		case Key::P:
			return "P";
		case Key::Q:
			return "Q";
		case Key::R:
			return "R";
		case Key::S:
			return "S";
		case Key::T:
			return "T";
		case Key::U:
			return "U";
		case Key::V:
			return "V";
		case Key::W:
			return "W";
		case Key::X:
			return "X";
		case Key::Y:
			return "Y";
		case Key::Z:
			return "Z";
		case Key::NUM0:
			return "NUM0";
		case Key::NUM1:
			return "NUM1";
		case Key::NUM2:
			return "NUM2";
		case Key::NUM3:
			return "NUM3";
		case Key::NUM4:
			return "NUM4";
		case Key::NUM5:
			return "NUM5";
		case Key::NUM6:
			return "NUM6";
		case Key::NUM7:
			return "NUM7";
		case Key::NUM8:
			return "NUM8";
		case Key::NUM9:
			return "NUM9";
		case Key::ESCAPE:
			return "ESCAPE";
		case Key::LCONTROL:
			return "LCONTROL";
		case Key::LSHIFT:
			return "LSHIFT";
		case Key::LALT:
			return "LALT";
		case Key::LSYSTEM:
			return "LSYSTEM";
		case Key::RCONTROL:
			return "RCONTROL";
		case Key::RSHIFT:
			return "RSHIFT";
		case Key::RALT:
			return "RALT";
		case Key::RSYSTEM:
			return "RSYSTEM";
		case Key::MENU:
			return "MENU";
		case Key::LBRACKET:
			return "LBRACKET";
		case Key::RBRACKET:
			return "RBRACKET";
		case Key::SEMICOLON:
			return "SEMICOLON";
		case Key::COMMA:
			return "COMMA";
		case Key::PERIOD:
			return "PERIOD";
		case Key::QUOTE:
			return "QUOTE";
		case Key::SLASH:
			return "SLASH";
		case Key::BACKSLASH:
			return "BACKSLASH";
		case Key::TILDE:
			return "TILDE";
		case Key::EQUAL:
			return "EQUAL";
		case Key::HYPHEN:
			return "HYPHEN";
		case Key::SPACE:
			return "SPACE";
		case Key::ENTER:
			return "ENTER";
		case Key::BACKSPACE:
			return "BACKSPACE";
		case Key::TAB:
			return "TAB";
		case Key::PAGEUP:
			return "PAGEUP";
		case Key::PAGEDOWN:
			return "PAGEDOWN";
		case Key::BEGIN:
			return "BEGIN";
		case Key::END:
			return "END";
		case Key::HOME:
			return "HOME";
		case Key::INSERT:
			return "INSERT";
		case Key::DELETE:
			return "DELETE";
		case Key::ADD:
			return "ADD";
		case Key::SUBSTRACT:
			return "SUBSTRACT";
		case Key::MULTIPLY:
			return "MULTIPLY";
		case Key::DIVIDE:
			return "DIVIDE";
		case Key::LEFT:
			return "LEFT";
		case Key::RIGHT:
			return "RIGHT";
		case Key::UP:
			return "UP";
		case Key::DOWN:
			return "DOWN";
		case Key::NUMPAD0:
			return "NUMPAD0";
		case Key::NUMPAD1:
			return "NUMPAD1";
		case Key::NUMPAD2:
			return "NUMPAD2";
		case Key::NUMPAD3:
			return "NUMPAD3";
		case Key::NUMPAD4:
			return "NUMPAD4";
		case Key::NUMPAD5:
			return "NUMPAD5";
		case Key::NUMPAD6:
			return "NUMPAD6";
		case Key::NUMPAD7:
			return "NUMPAD7";
		case Key::NUMPAD8:
			return "NUMPAD8";
		case Key::NUMPAD9:
			return "NUMPAD9";
		case Key::F1:
			return "F1";
		case Key::F2:
			return "F2";
		case Key::F3:
			return "F3";
		case Key::F4:
			return "F4";
		case Key::F5:
			return "F5";
		case Key::F6:
			return "F6";
		case Key::F7:
			return "F7";
		case Key::F8:
			return "F8";
		case Key::F9:
			return "F9";
		case Key::F10:
			return "F10";
		case Key::F11:
			return "F11";
		case Key::F12:
			return "F12";
		case Key::F13:
			return "F13";
		case Key::F14:
			return "F14";
		case Key::F15:
			return "F15";
		case Key::PAUSE:
			return "PAUSE";
		case Key::KEYCOUNT:
			return "KEYCOUNT";
		}

		return "UNKNOWN KEY";
	}

} // namespace storm::window

CUSTOM_FORMAT(storm::window::Key, "{}", storm::window::keyToString(data));
