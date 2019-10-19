#pragma once
#include <storm/core/Configure.hpp>

#if __has_include(<execution>)
#include <execution>
namespace _std {
	using any = std::execution;
}
#elif __has_include(<experimental/execution>)
#include <experimental/execution>
namespace _std {
	using execution = std::experimental::execution;
}
#endif
