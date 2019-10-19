// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>

#ifdef STORM_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

#include <functional>
#include <storm/core/Filesystem.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/module/ForwardDeclarations.hpp>

namespace storm::module {
	class STORM_PUBLIC STORM_EBCO AbstractModule : public core::NonCopyable {
	  public:
		explicit AbstractModule(core::filesystem::path filepath);
		virtual ~AbstractModule();

		AbstractModule(AbstractModule &&);
		AbstractModule &operator=(AbstractModule &&);

		inline bool isLoaded() const noexcept {
			return m_is_loaded;
		}

		template <class Signature>
		inline std::function<Signature> getFunc(std::string_view name) {
			return std::function<Signature>(
				reinterpret_cast<Signature *>(_getFunc(name)));
		}

		template <class Signature>
		inline Signature getCFunc(std::string_view name) {
			return reinterpret_cast<Signature>(_getFunc(name));
		}

		inline const core::filesystem::path &getFilepath() const {
			return m_filepath;
		}

	  protected:
		virtual void *_getFunc(std::string_view name) const = 0;

		bool m_is_loaded = false;

		core::filesystem::path m_filepath;
	};
} // namespace storm::module

#ifdef STORM_COMPILER_MSVC
#pragma warning(pop)
#endif
