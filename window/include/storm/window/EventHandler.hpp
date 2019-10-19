// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <unordered_map>
#include <vector>

#include <storm/core/Platform.hpp>
#include <storm/window/ForwardDeclarations.hpp>

namespace storm::window {
	class STORM_PUBLIC EventHandler {
	  public:
		using Callback = std::function<void(const Event &)>;

		explicit EventHandler(Window &window);
		~EventHandler();

		EventHandler(const EventHandler &);
		EventHandler &operator=(const EventHandler &);

		EventHandler(EventHandler &&);
		EventHandler &operator=(EventHandler &&);

		void update();

		inline void addCallback(EventType event_type, Callback callback) {
			m_callback[event_type].emplace_back(std::move(callback));
		}

		inline void clearCallbacks(EventType event_type) {
			m_callback[event_type].clear();
		}

	  private:
		WindowObserverPtr m_window;

		std::unordered_map<EventType, std::vector<Callback>> m_callback;
	};
} // namespace storm::window
