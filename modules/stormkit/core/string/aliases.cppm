// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

export module stormkit.core:string.aliases;

import std;

namespace stdp = std::pmr;

export namespace stormkit { inline namespace core {
    using std::string;
    using std::string_view;
    using std::u16string;
    using std::u16string_view;
    using std::u32string;
    using std::u32string_view;
    using std::u8string;
    using std::u8string_view;
    using std::wstring;
    using std::wstring_view;

    namespace pmr {
        using stdp::string;
        using stdp::u16string;
        using stdp::u32string;
        using stdp::u8string;
        using stdp::wstring;
    } // namespace pmr

    using czstring  = const char*;
    using zstring   = char*;
    using cwzstring = const wchar_t*;
    using wzstring  = wchar_t*;
}} // namespace stormkit::core
