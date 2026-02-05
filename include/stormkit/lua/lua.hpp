#ifndef STORMKIT_LUA_HPP
#define STORMKIT_LUA_HPP

#include <stormkit/core/platform_macro.hpp>

STORMKIT_PUSH_WARNINGS

#define LUA_API extern __attribute__((visibility("default")))

extern "C" {
#include <lua.h>

#include <luacode.h>
#include <lualib.h>
}

#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#undef lua_rawgetp
#undef lua_rawsetp
#include <LuaBridge/LuaBridge.h>
#undef assert

STORMKIT_POP_WARNINGS

#endif
