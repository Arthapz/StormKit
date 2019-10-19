// Copryright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/// \file Flags.hpp
/// \author Arthapz
/// \brief this file provide a macro to define enum class with binary operators

#pragma once

#include <storm/core/private/Flags.hpp>

/// \brief FLAG_ENUM is a macro which define enum class with binary operators
/// \param x the type of enum
#define FLAG_ENUM(x)                                    \
    template<>                                          \
    struct enable_bitmask_operators<x> {                \
        constexpr enable_bitmask_operators() = default; \
        static const bool enable             = true;    \
    };

namespace storm::core {
    template<typename Enum, typename Enum2>
    constexpr inline bool checkFlag(Enum value, Enum2 flag) {
        return (value & flag) == value;
    }
} // namespace storm::core
