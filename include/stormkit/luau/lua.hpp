#ifndef STORMKIT_LUA_HPP
#define STORMKIT_LUA_HPP

#include <stormkit/core/platform_macro.hpp>

STORMKIT_PUSH_WARNINGS

extern "C" {
#include <lua.h>

#include <luacode.h>
#include <lualib.h>
}

#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <LuaBridge/LuaBridge.h>
#undef assert

STORMKIT_POP_WARNINGS

#endif
