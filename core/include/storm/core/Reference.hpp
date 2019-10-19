// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <functional>
#include <storm/core/NonCopyable.hpp>

namespace storm::core {
	template <typename... T> class Reference {};

	template <typename U, typename T, typename... Others>
	class ReferenceResolve {
	  public:
		inline const U &operator()(const Reference<T, Others...> &owner);
		inline U &operator()(Reference<T, Others...> &owner);
	};

	template <typename T, typename... Others>
	class ReferenceResolve<T, T, Others...> {
	  public:
		inline const T &operator()(const Reference<T, Others...> &owner);
		inline T &operator()(Reference<T, Others...> &owner);
	};

	template <typename T, typename... Others>
	class Reference<T, Others...> : public core::NonCopyable {
	  public:
		explicit Reference(T &value, Others &... others);
		~Reference();

		Reference(Reference &&);
		Reference &operator=(Reference &&);

		template <typename U> inline U &get();

		template <typename U> inline const U &get() const;

		template <typename U, typename V> inline V &getAs();

		template <typename U, typename V> inline const V &getAs() const;

	  private:
		template <typename U, typename _T, typename... _Others>
		friend class ReferenceResolve;

		std::reference_wrapper<T> m_value;
		Reference<Others...> m_next;
	};

} // namespace storm::core

#include "Reference.inl"
