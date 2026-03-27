// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

#ifdef STORMKIT_OS_WINDOWS
    #include <stormkit/core/platform/windows.hpp>
#endif

export module stormkit.test;

import stormkit.core;
import std;
import frozen;

using namespace stormkit;

export namespace test {
    struct TestFunc {
        string                name;
        std::function<void()> func;
    };

    struct TestSuiteHolder {
        auto                 hasTest(string_view name) noexcept;
        auto                 runTest(string_view name) noexcept;
        auto                 runTests() noexcept;
        string               name;
        dyn_array<TestFunc>  tests;
        std::source_location location;
    };

    struct TestSuite {
        TestSuite(string&&                    name,
                  dyn_array<TestFunc>&&       tests,
                  const std::source_location& location = std::source_location::current()) noexcept;
    };

    auto expects(bool cond, string_view message, const std::source_location& location = std::source_location::current()) noexcept;

    auto parse_args(array_view<const string_view> args) noexcept -> void;
    auto runTests() noexcept -> int;
} // namespace test

module :private;

using namespace std::literals;

namespace test {
    enum class Status {
        Passed,
        NotPassed,
        CheckMark,
        CrossMark,
    };

    struct TestState {
        dyn_array<std::unique_ptr<TestSuiteHolder>> test_suites;
        bool                                        verbose        = false;
        bool                                        failed         = false;
        bool                                        plain          = false;
        std::optional<string>                       requested_test = std::nullopt;
    };

    namespace {
        constexpr auto StyleMap = frozen::make_unordered_map<Status, ConsoleStyle>({
          { Status::Passed,    ConsoleStyle { .fg = ConsoleColor::BLACK, .bg = ConsoleColor::GREEN } },
          { Status::NotPassed, ConsoleStyle { .fg = ConsoleColor::BLACK, .bg = ConsoleColor::RED }   },
          { Status::CheckMark,
           ConsoleStyle {
              .fg = ConsoleColor::GREEN,
            }                                                                                        },
          { Status::CrossMark,
           ConsoleStyle {
              .fg = ConsoleColor::RED,
            }                                                                                        },
        });

        constexpr auto passed     = StyleMap.at(Status::Passed) | "Passed"sv;
        constexpr auto not_passed = StyleMap.at(Status::NotPassed) | "Not Passed"sv;
        constexpr auto check_mark = StyleMap.at(Status::CheckMark) | "✔ "sv;
        constexpr auto cross_mark = StyleMap.at(Status::CrossMark) | "❌"sv;
    } // namespace

    auto state = TestState {};

    auto TestSuiteHolder::hasTest(string_view _name) noexcept {
        for (auto&& test : tests) {
            if (test.name == _name) return true;
        }

        return false;
    }

    auto TestSuiteHolder::runTest(string_view _name) noexcept {
        for (auto&& test : tests) {
            if (test.name == _name) {
                if (state.verbose) std::println("     running test {}", test.name);
                test.func();
                std::println("     {}", (state.failed) ? not_passed : passed);
                return not state.failed;
            }
        }
        return true;
    }

    auto TestSuiteHolder::runTests() noexcept {
        if (state.verbose) { std::println("   > file: {}", location.file_name()); }

        auto passed_tests = 0;
        auto failed_tests = 0;

        for (auto&& test : tests) {
            state.failed = false;
            if (state.verbose) std::println("     running test {}", test.name);
            test.func();

            if (state.verbose) std::println("     {}", (state.failed) ? not_passed : passed);
            if (not state.failed) ++passed_tests;
            else
                ++failed_tests;
        }

        if (not state.plain) std::print("{}", check_mark);
        std::print("{} test passed", passed_tests);
        if (failed_tests > 0) {
            std::print(", ");
            if (not state.plain) std::print("{}", cross_mark);
            std::print("{} test state.failed", failed_tests);
        }
        std::println("");

        return failed_tests == 0;
    }

    TestSuite::TestSuite(string&& _name, dyn_array<TestFunc>&& tests, const std::source_location& location) noexcept {
        state.test_suites.emplace_back(std::make_unique<TestSuiteHolder>(std::move(_name), std::move(tests), location));
    }

    auto expects(bool cond, string_view message, const std::source_location& location) noexcept {
        if (not cond) [[unlikely]] {
            state.failed = true;
            if (state.verbose) {
                std::println("{} on expression \"{}\" \n"
                             "          > line: {}",
                             StyleMap.at(Status::NotPassed) | "FAILURE",
                             message,
                             location.line());
            }
        }
    }

    auto split(string_view str, char delim) noexcept -> dyn_array<string> {
        auto output = dyn_array<string> {};
        auto first  = std::size_t { 0u };

        while (first < str.size()) {
            const auto second = str.find_first_of(delim, first);

            if (first != second) output.emplace_back(str.substr(first, second - first));

            if (second == string_view::npos) break;

            first = second + 1;
        }

        return output;
    }

    auto parse_args(array_view<const string_view> args) noexcept -> void {
        for (auto&& arg : args) {
            if (arg == "--verbose" or arg == "-v") state.verbose = true;
            else if (arg == "--plain" or arg == "-p")
                state.plain = true;
            else if (arg.starts_with("--test_name=")) { state.requested_test = split(arg, '=')[1]; }
        }
    }

    auto runTests() noexcept -> int {
        auto return_code = 0;

        if (state.requested_test) {
            for (auto&& suite : state.test_suites) {
                if (suite->hasTest(*state.requested_test)) {
                    if (not suite->runTest(*state.requested_test)) return_code = -1;
                    break;
                }
            }
        } else
            for (auto&& suite : state.test_suites) {
                std::println("Running test suite {} ({} tests)", suite->name, std::size(suite->tests));
                if (not suite->runTests()) return_code = -1;
            }

        return return_code;
    }
} // namespace test
