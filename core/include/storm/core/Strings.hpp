// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <fmt/core.h>
#include <fmt/format.h>
#include <functional>
#include <gsl/string_span>
#include <iomanip>
#include <locale>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <storm/core/Assert.hpp>
#include <storm/core/TypeTraits.hpp>
#include <string>
#include <string_view>
#include <type_traits>

#define STRINGIFY(x) #x
#define CASE(x)                                                                \
	case x:                                                                    \
		return STRINGIFY(x);

#define CUSTOM_FORMAT(TYPE, OUTPUT_STR, ...)                                   \
	namespace fmt {                                                            \
		template <> struct formatter<TYPE> {                                   \
			template <typename ParseContext>                                   \
			constexpr auto parse(ParseContext &ctx) {                          \
				return ctx.begin();                                            \
			}                                                                  \
			template <typename FormatContext>                                  \
			auto format(const TYPE &data, FormatContext &ctx) {                \
				return format_to(ctx.out(), OUTPUT_STR, __VA_ARGS__);          \
			}                                                                  \
		};                                                                     \
	}

namespace storm::core {
    inline std::vector<std::string> split(std::string_view string, char delim) {
        auto output = std::vector<std::string>{};
        auto first	= size_t{0u};

		while (first < string.size()) {
			const auto second = string.find_first_of(delim, first);

			if (first != second)
                output.emplace_back(string.substr(first, second - first));

			if (second == std::string_view::npos)
				break;

			first = second + 1;
		}

		return output;
	}

	template <typename T, bool complete = true> std::string intToHex(T i) {
		static_assert(
			std::is_integral<T>::value,
			"Template argument 'T' must be a fundamental integer type (e.g. "
			"int, short, etc..).");

		auto stream = std::stringstream{};

		if constexpr (complete)
			stream << "0x" << std::setfill('0') << std::setw(sizeof(T) * 2u)
				   << std::hex;
		else
			stream << std::hex;

		if constexpr (std::is_same_v<std::uint8_t, T>)
			stream << static_cast<std::int32_t>(i);
		else if constexpr (std::is_same_v<std::int8_t, T>)
			stream << static_cast<std::int32_t>(static_cast<uint8_t>(i));
		else
			stream << i;

		return stream.str();
	}

	template <typename String = std::string>
	inline auto toLower(String &&string, std::locale locale = std::locale("")) {
		using StringRawType = std::remove_reference_t<String>;

		auto &f =
			std::use_facet<std::ctype<typename StringRawType::value_type>>(
				locale);

		auto result = StringRawType{std::forward<String>(string)};
		f.tolower(&result[0], &result[0] + result.size());

		return result;
	}

	template <typename String = std::string>
	inline String toUpper(String &&string,
						  std::locale locale = std::locale("")) {
		using StringRawType = std::remove_reference_t<String>;

		auto &f =
			std::use_facet<std::ctype<typename StringRawType::value_type>>(
				locale);

		auto result = StringRawType{std::forward<String>(string)};
		f.toupper(&result[0], &result[0] + result.size());

		return result;
	}
} // namespace storm::core
