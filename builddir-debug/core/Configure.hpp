#pragma once

#include <string_view>

static constexpr auto STORM_VERSION = std::string_view{"0.1.0"};
static constexpr auto STORM_MAJOR_VERSION = 0;
static constexpr auto STORM_MINOR_VERSION = 1;
static constexpr auto STORM_PATCH_VERSION = 0;
static constexpr auto STORM_MILESTONE = std::string_view{"alpha"};
static constexpr auto STORM_BUILD_TYPE = std::string_view{"debug"};
static constexpr auto STORM_GIT_COMMIT_HASH = std::string_view{"c939d48"};
static constexpr auto STORM_GIT_BRANCH = std::string_view{"dev"};