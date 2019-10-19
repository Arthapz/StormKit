// Copryright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/// \file Observer.hpp
/// \author Arthapz
///
#pragma once

#include <functional>
#include <storm/core/Memory.hpp>
#include <string>

/// \module storm::core
namespace storm::core {
    /// \brief storm::core::Observer<Event, CustomData> is a utility class to
    /// declare an Observer type (Observer pattern)
    template<typename Event, typename CustomData>
    class Observer {
      public:
        /// \output_section Public Special Member Functions
        /// \brief Default constructor
        explicit Observer() noexcept = default;

        /// \brief Destructor
        virtual ~Observer() noexcept = default;

        /// \brief Move constructor
        Observer(Observer &&) noexcept = default;

        /// \brief Move assignement
        Observer &operator=(Observer &&) noexcept = default;

        /// \brief Copy constructor
        Observer(const Observer &) noexcept = default;

        /// \brief Copy assignement
        Observer &operator=(const Observer &) noexcept = default;

        // TODO continue DOCUMENTATION
        virtual void onNotified(Event, CustomData &&) = 0;
    };
} // namespace storm::core
