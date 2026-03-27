// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform/windows.hpp>

#include <WinNT.h>

module stormkit.core;

import :containers.shmbuffer;

namespace stormkit { inline namespace core {
    /////////////////////////////////////
    /////////////////////////////////////
    SHMBuffer::~SHMBuffer() {
        if (m_handle) {
            UnmapViewOfFile(stdr::data(m_data));
            CloseHandle(std::bit_cast<HANDLE>(m_handle));
            m_handle = nullptr;
            m_size   = 0;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto SHMBuffer::do_init(PrivateTag, usize size, string name, io::Access access) noexcept
      -> std::expected<void, std::error_code> {
        m_size                 = size;
        m_name                 = std::move(name);
        m_access               = access;
        const auto page_access = (check_flag_bit(m_access, io::Access::WRITE) ? PAGE_READWRITE : PAGE_READONLY);

        // TODO handle reallocation
        m_handle = ::CreateFileMapping(INVALID_HANDLE_VALUE,
                                       nullptr,
                                       as<DWORD>(page_access),
                                       0,
                                       as<DWORD>(m_size),
                                       stdr::data(m_name));
        if (m_handle == nullptr)
            return std::unexpected {
                std::error_code { as<i32>(GetLastError()), std::system_category() }
            };

        const auto file_access = init_by<u32>([access = m_access](auto& file_access) noexcept {
            if (check_flag_bit(access, io::Access::READ)) file_access |= FILE_MAP_READ;
            if (check_flag_bit(access, io::Access::WRITE)) file_access |= FILE_MAP_WRITE;
        });

        auto buf = ::MapViewOfFile(m_handle, file_access, 0, 0, as<DWORD>(m_size));
        if (buf == nullptr)
            return std::unexpected {
                std::error_code { as<i32>(GetLastError()), std::system_category() }
            };

        m_data = { std::bit_cast<byte*>(buf), m_size };

        return {};
    }
}} // namespace stormkit::core
