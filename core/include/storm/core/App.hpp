// Copryright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/// \file App.hpp
/// \author Arthapz

#pragma once

#include <storm/core/NonCopyable.hpp>
#include <storm/core/NonMovable.hpp>

/// \module storm::core
namespace storm::core {
    /// \brief storm::core::App is an abstract class that encapsulate all the
    /// app logic
    ///
    /// This class is a facility to keep the code aside from the main and split
    /// it in OO way
    class App: public storm::core::NonCopyable, public storm::core::NonMovable {
      public:
        /// \output_section Public Special Member Functions
        /// \brief Default constructor
        explicit App() noexcept = default;

        /// \brief Destructor
        virtual ~App() noexcept = default;

        /// \output_section Public Member Functions
        /// \brief Run the app logic
        ///
        /// This is a pure virtual function that has to be implemented by the
        /// derived class to define the app logic
        virtual void run(int argc, char **argv) = 0;
    };
} // namespace storm::core
