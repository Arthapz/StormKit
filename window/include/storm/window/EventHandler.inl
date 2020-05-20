// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::window {
    /////////////////////////////////////
    /////////////////////////////////////
     void EventHandler::addCallback(EventType event_type, Callback callback) {
        m_callback[event_type].emplace_back(std::move(callback));
    }

    /////////////////////////////////////
    /////////////////////////////////////
     void EventHandler::clearCallbacks(EventType event_type) {
        m_callback[event_type].clear();
    }
} // namespace storm::window
