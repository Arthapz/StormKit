// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/NamedType.hpp>

namespace storm::log {
	using Module = core::NamedType<const char *, struct LogModuleParameter>;

	constexpr storm::log::Module
		operator"" _module(const char *str, [[maybe_unused]] std::size_t sz) {
		return storm::core::makeNamed<Module>(str);
	}
} // namespace storm::log
