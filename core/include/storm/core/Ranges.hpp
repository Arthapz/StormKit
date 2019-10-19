// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/// \file FileSystem.hpp
/// \author Arthapz
/// \brief this file map std::filesystem or std::filesystem

#pragma once

#include <storm/core/Configure.hpp>
#include <storm/core/Platform.hpp>

#if __has_include(<ranges>) && defined(__cpp_lib_concepts)
    #include <ranges>
namespace storm::core {
    namespace ranges = std::ranges;
} // namespace storm::core
#elif __has_include(<experimental/ranges>)
    #include <experimental/ranges>
namespace storm::core {
    namespace ranges = std::experimental::ranges;
} // namespace storm::core
#elif __has_include(<range/v3/all.hpp>)
    #include <range/v3/all.hpp>
namespace storm::core {
    namespace ranges = ranges;
} // namespace storm::core
#endif
