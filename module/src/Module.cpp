// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/module/Module.hpp>

#if defined(STORM_OS_WINDOWS)
#include "win32/ModuleImpl.hpp"
using Impl = storm::module::ModuleImpl;
#elif defined(STORM_POSIX)
#include "posix/ModuleImpl.hpp"
using Impl = storm::module::ModuleImpl;
#endif

using namespace storm;
using namespace storm::module;

/////////////////////////////////////
/////////////////////////////////////
Module::Module(core::filesystem::path filename) : m_impl{nullptr} {
	STORM_EXPECTS(!filename.empty());

	m_impl = std::make_unique<Impl>(filename);
}

/////////////////////////////////////
/////////////////////////////////////
Module::~Module() = default;

/////////////////////////////////////
/////////////////////////////////////
Module::Module(Module &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Module &Module::operator=(Module &&) = default;
