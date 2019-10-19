// Copryright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <optional>

/// \module storm::core
namespace storm::core {
    template<typename T>
    using DeferredStackAlloc = std::optional<T>;
} // namespace storm::core
