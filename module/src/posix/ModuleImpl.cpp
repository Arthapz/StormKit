// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "ModuleImpl.hpp"

#include <dlfcn.h>
#include <storm/log/LogHandler.hpp>

using namespace storm;
using namespace storm::module;

////////////////////////////////////////
////////////////////////////////////////
ModuleImpl::ModuleImpl(core::filesystem::path filepath)
	: AbstractModule{std::move(filepath)} {
	m_library_handle = dlopen(m_filepath.c_str(), RTLD_LAZY | RTLD_LOCAL);

	if (!m_library_handle) {
		auto system_reason = std::string(dlerror());
		log::LogHandler::elog("Failed to load module \"%{1}\", reason: %{2}",
							  m_filepath.string(), system_reason);
	} else
		m_is_loaded = true;
}

////////////////////////////////////////
////////////////////////////////////////
ModuleImpl::~ModuleImpl() {
	if (m_library_handle)
		dlclose(m_library_handle);
}

/////////////////////////////////////
/////////////////////////////////////
ModuleImpl::ModuleImpl(ModuleImpl &&) = default;

/////////////////////////////////////
/////////////////////////////////////
ModuleImpl &ModuleImpl::operator=(ModuleImpl &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void *ModuleImpl::_getFunc(std::string_view name) const {
	void *func = dlsym(m_library_handle, name.data());

	if (!func) {
		auto system_reason = std::string(dlerror());
		log::LogHandler::elog("Failed to load function \"%{1}()\" (on module "
							  "\"%{2}\"), reason: %{3}",
							  name, m_filepath.string(), system_reason);
	}

	return func;
}
