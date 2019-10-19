// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <gsl/gsl_assert>
#include <storm/core/Assert.hpp>
#include <storm/core/Platform.hpp>
#include <storm/log/LogHandler.hpp>
#include <storm/module/AbstractModule.hpp>

#ifdef STORM_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

/**
 * @file SharedLibrary.hpp
 * @brief Shared Library tools
 * @author Arthapz
 * @version 0.1
 **/

namespace storm::module {

	constexpr const static auto LOG_MODULE_MODULE =
		core::makeNamed<log::Module>("Stormkit-Module");
	/**
	 * @class SharedLibrary
	 * @ingroup Core
	 * @brief Shared Library representation class
	 *
	 * The class allow a simple access cross-os to a shared library
	 */
	class STORM_PUBLIC STORM_EBCO Module : public core::NonCopyable {
	  public:
		/**
		 * @brief Constructor
		 *
		 * Constructor of SharedLibrary
		 *
		 * @param filename : the filename of the shared library (string)
		 * assert(the filename can't be empty)
		 */
		explicit Module(core::filesystem::path filename);

		/**
		 * @brief Destructor
		 *
		 * Destructor of SharedLibrary
		 */
		~Module();

		Module(Module &&);
		Module &operator=(Module &&);

		inline bool isLoaded() const noexcept {
			return m_impl->isLoaded();
		}

		/**
		 * @brief Get a shared symbol
		 *
		 * Method wich load and return the shared symbol of the shared library
		 * currently loaded
		 *
		 * @param name : the name of the shared symbol (string) assert(the name
		 * can't be empty)
		 */
		template <class Signature>
		std::function<Signature> getFunc(std::string_view name) const {
			STORM_EXPECTS(!name.empty());

			return m_impl->getFunc<Signature>(name);
		}

		template <class Signature>
		Signature getCFunc(std::string_view name) const {
			STORM_EXPECTS(!name.empty());

			return m_impl->getCFunc<Signature>(name);
		}

	  private:
		AbstractModuleOwnedPtr m_impl;
	};
} // namespace storm::module

#ifdef STORM_COMPILER_MSVC
#pragma warning(pop)
#endif
