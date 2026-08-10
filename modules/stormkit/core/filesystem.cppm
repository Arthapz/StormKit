// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/memory_macro.hpp>
#include <stormkit/core/platform_macro.hpp>

#include <stormkit/core/try_expected.hpp>

#ifdef STORMKIT_OS_WINDOWS
    #include <stormkit/core/platform/windows.hpp>

    #include <fcntl.h>
    #include <io.h>
    #include <sys/stat.h>
#else
    #include <fcntl.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <unistd.h>
#endif

#include <cstdio>

export module stormkit.core.filesystem;

import std;

import stormkit.core.errors;
import stormkit.core.contract;
import stormkit.core.heap;
import stormkit.core.functional;
import stormkit.core.meta;
import stormkit.core.types;
import stormkit.core.typesafe.flags;
import stormkit.core.typesafe.safecasts;
import stormkit.core.containers.safecasts;

export {
    namespace stormkit { inline namespace core { namespace io {
        enum class Mode {
            BINARY,
            AINSI,
            UTF8,
            WIDE,
        };
    }}} // namespace stormkit::core::io
}

namespace stormkit { inline namespace core { namespace io::meta {
    template<Mode mode>
        requires(mode != Mode::BINARY)
    using ModeToCharType = core::meta::lazy_conditional<mode == Mode::WIDE, wchar, char>;
}}} // namespace stormkit::core::io::meta

export {
    namespace stormkit { inline namespace core {
        namespace stdfs = std::filesystem;

        namespace io {

            enum class Access {
                READ,
                WRITE,
            };

            template<Mode mode>
            class Descriptor final {
                struct Private_tag {};

              public:
#ifdef STORMKIT_OS_WINDOWS
                static inline const auto INVALID_HANDLE = INVALID_HANDLE_VALUE;
                using Native_handle                     = HANDLE;
#else
                static constexpr auto INVALID_HANDLE = 0;
                using Native_handle                  = i32;
#endif

                explicit Descriptor(Private_tag) noexcept;
                ~Descriptor() noexcept;

                Descriptor(Descriptor&)                    = delete;
                auto operator=(Descriptor&) -> Descriptor& = delete;

                Descriptor(Descriptor&&) noexcept;
                auto operator=(Descriptor&&) noexcept -> Descriptor&;

                static auto open(const stdfs::path& path, Access access) noexcept -> System_result<Descriptor>;
                static auto allocate_and_open(const stdfs::path& path, Access access) noexcept
                  -> System_result<heap_ptr<Descriptor>>;

                auto close() noexcept;

                auto read_to(array_view<byte> out) noexcept -> System_result<usize>;
                auto read_to(array_view<char> out) noexcept -> System_result<usize>
                    requires(mode == Mode::UTF8 or mode == Mode::AINSI);
                auto read_to(array_view<wchar> out) noexcept -> System_result<usize>
                    requires(mode == Mode::WIDE);

                auto write(array_view<byte> bytes) noexcept -> System_result<usize>;
                auto write(array_view<const char> bytes) noexcept -> System_result<usize>
                    requires(mode == Mode::UTF8 or mode == Mode::AINSI);
                auto write(array_view<const wchar> bytes) noexcept -> System_result<usize>
                    requires(mode == Mode::WIDE);

                auto flush() noexcept -> void;

                auto position() const noexcept -> usize;
                auto size() const noexcept -> usize;

                auto native_descriptor() const noexcept -> Native_handle;

                auto do_init(Private_tag, const stdfs::path&, Access) noexcept -> System_result<void>;

              private:
                Native_handle              m_descriptor = INVALID_HANDLE;
                mutable std::atomic<usize> m_size       = 0;
            };

            using File = Descriptor<Mode::BINARY>;
            template<Mode mode = Mode::UTF8>
                requires(mode != Mode::BINARY)
            using TextFile = Descriptor<mode>;

            template<Mode mode = Mode::UTF8>
            auto readfile_to(const stdfs::path& path, array_view<meta::ModeToCharType<mode>> output) noexcept
              -> System_result<usize>;
            template<Mode mode = Mode::UTF8>
            auto readfile(const stdfs::path& path) noexcept -> System_result<dynarray<meta::ModeToCharType<mode>>>;

            auto readfile_to(const stdfs::path& path, array_view<byte> output) noexcept -> System_result<usize>;
            auto readfile(const stdfs::path& path) noexcept -> System_result<dynarray<byte>>;

            template<Mode mode = Mode::UTF8>
            auto writefile(const stdfs::path& path, array_view<const meta::ModeToCharType<mode>> data) noexcept
              -> System_result<usize>;
            auto writefile(const stdfs::path& path, array_view<byte> data) noexcept -> System_result<usize>;
        } // namespace io
    }} // namespace stormkit::core

    template<>
    inline constexpr auto stormkit::meta::FLAG_TRAIT<stormkit::io::Access> = true;
} // namespace stormkit::core::io

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stdr = std::ranges;

namespace stormkit { inline namespace core { namespace io {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline Descriptor<mode>::Descriptor(Private_tag) noexcept {
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline Descriptor<mode>::~Descriptor() noexcept {
        close();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline Descriptor<mode>::Descriptor(Descriptor&& other) noexcept
        : m_descriptor { std::exchange(other.m_descriptor, INVALID_HANDLE) } {
        m_size = other.m_size.load();
        other.m_size.store(0);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline auto Descriptor<mode>::operator=(Descriptor&& other) noexcept -> Descriptor& {
        if (this == &other) [[unlikely]]
            return *this;

        m_descriptor = std::exchange(other.m_descriptor, INVALID_HANDLE);
        m_size       = other.m_size.load();
        other.m_size.store(0);

        return *this;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline auto Descriptor<mode>::open(const stdfs::path& path, Access access) noexcept -> System_result<Descriptor<mode>> {
        auto out = Descriptor<mode> { Private_tag {} };
        out.do_init(Private_tag {}, path, access);
        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline auto Descriptor<mode>::allocate_and_open(const stdfs::path& path, Access access) noexcept
      -> System_result<heap_ptr<Descriptor<mode>>> {
        auto out = allocate_unsafe<Descriptor<mode>>(Private_tag {});
        out->do_init(Private_tag {}, path, access);
        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline auto Descriptor<mode>::close() noexcept {
        if (m_descriptor != INVALID_HANDLE) {
            flush();
#ifdef STORMKIT_OS_WINDOWS
            CloseHandle
#else
            ::close
#endif
              (m_descriptor);
            m_descriptor = INVALID_HANDLE;
            m_size       = 0;
        }
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline auto Descriptor<mode>::read_to(array_view<byte> out) noexcept -> System_result<usize> {
        EXPECTS(m_descriptor != INVALID_HANDLE);
#ifdef STORMKIT_OS_WINDOWS
        // TODO support async WriteFileEx
        auto ret = DWORD { 0 };
        const auto
          succeed = ReadFile(m_descriptor, std::bit_cast<void*>(stdr::data(out)), as<DWORD>(stdr::size(out)), &ret, nullptr);
        if (not succeed) return std::unexpected { error_code::from_win32() };
#else
        const auto ret = ::read(m_descriptor, std::bit_cast<void*>(stdr::data(out)), as<u32>(stdr::size(out)));
        if (ret == -1) return std::unexpected { error_code::from_errno() };
#endif

        return System_result<usize> { std::in_place, as<usize>(ret) };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline auto Descriptor<mode>::read_to(array_view<char> out) noexcept -> System_result<usize>
        requires(mode == Mode::UTF8 or mode == Mode::AINSI)
    {
        return read_to(as<Bytes>(out));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline auto Descriptor<mode>::read_to(array_view<wchar> out) noexcept -> System_result<usize>
        requires(mode == Mode::WIDE)
    {
        return read_to(as<Bytes>(out));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline auto Descriptor<mode>::write(array_view<byte> data) noexcept -> System_result<usize> {
        EXPECTS(m_descriptor != INVALID_HANDLE);
#ifdef STORMKIT_OS_WINDOWS
        // TODO support async WriteFileEx
        auto       ret     = DWORD { 0 };
        const auto succeed = WriteFile(m_descriptor,
                                       std::bit_cast<const void*>(stdr::data(data)),
                                       as<DWORD>(stdr::size(data)),
                                       &ret,
                                       nullptr);
        if (not succeed) return std::unexpected { error_code::from_win32() };
#else
        const auto ret = ::write(m_descriptor, std::bit_cast<const void*>(stdr::data(data)), as<u32>(stdr::size(data)));
        if (ret == -1) return std::unexpected { error_code::from_errno() };
#endif

        return System_result<usize> { std::in_place, as<usize>(ret) };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline auto Descriptor<mode>::write(array_view<const char> data) noexcept -> System_result<usize>
        requires(mode == Mode::UTF8 or mode == Mode::AINSI)
    {
        return write(as<Bytes>(data));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline auto Descriptor<mode>::write(array_view<const wchar> data) noexcept -> System_result<usize>
        requires(mode == Mode::WIDE)
    {
        return write(as<Bytes>(data));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline auto Descriptor<mode>::flush() noexcept -> void {
        EXPECTS(m_descriptor != INVALID_HANDLE);
#ifdef STORMKIT_OS_WINDOWS
        FlushFileBuffers(reinterpret_cast<HANDLE>(m_descriptor));
#else
    #ifdef STORMKIT_OS_LINUX
        fdatasync
    #else
        fsync
    #endif
          (m_descriptor);
#endif
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline auto Descriptor<mode>::position() const noexcept -> usize {
        EXPECTS(m_descriptor != INVALID_HANDLE);
#ifdef STORMKIT_OS_WINDOWS
        auto current_position = LARGE_INTEGER { .QuadPart = 0 };
        SetFilePointerEx(m_descriptor, LARGE_INTEGER { .QuadPart = 0 }, &current_position, FILE_CURRENT);
        return as<usize>(current_position.QuadPart);
#else
        return as<usize>(lseek(m_descriptor, 0, SEEK_CUR));
#endif
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    inline auto Descriptor<mode>::size() const noexcept -> usize {
        EXPECTS(m_descriptor != INVALID_HANDLE);
        if (m_size == 0) {
#ifdef STORMKIT_OS_WINDOWS
            auto       size    = LARGE_INTEGER { .QuadPart = 0 };
            const auto succeed = GetFileSizeEx(m_descriptor, &size);
            if (succeed) m_size = as<usize>(size.QuadPart);
#else
            lseek(m_descriptor, 0, SEEK_SET);
            m_size = as<usize>(lseek(m_descriptor, 0, SEEK_END));
            lseek(m_descriptor, as<i32>(old_pos), SEEK_SET);
#endif
        }

        return m_size;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline auto Descriptor<mode>::native_descriptor() const noexcept -> Native_handle {
        EXPECTS(m_descriptor != INVALID_HANDLE);
        return m_descriptor;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    inline auto Descriptor<mode>::do_init(Private_tag, const stdfs::path& path, Access access) noexcept -> System_result<void> {
        if (access == Access::READ and not stdfs::exists(path))
            return std::unexpected { error_code::from_stderrc(std::errc::no_such_file_or_directory) };

        if (stdfs::is_directory(path)) return std::unexpected { error_code::from_stderrc(std::errc::is_a_directory) };

#ifdef STORMKIT_OS_WINDOWS
        const auto path_ = "\\\\?\\" / path;

        const auto win32_access = [&access]() noexcept -> DWORD {
            if (access == Access::READ) return GENERIC_READ;
            else if (access == Access::WRITE)
                return GENERIC_WRITE;
            else
                return GENERIC_WRITE | GENERIC_READ;
            std::unreachable();
        }();
        const auto path_as_string = path_.string();
        const auto ret            = CreateFile(stdr::data(path_as_string),
                                               win32_access,
                                               FILE_SHARE_READ,
                                               nullptr,
                                               OPEN_ALWAYS,
                                               FILE_ATTRIBUTE_NORMAL,
                                               INVALID_HANDLE_VALUE);
        if (ret == INVALID_HANDLE_VALUE) return std::unexpected { error_code::from_win32() };
#else
        const auto posix_access = [&access]() noexcept {
            if (access == Access::READ) return O_RDONLY;
            else if (access == Access::WRITE)
                return O_WRONLY | O_CREAT;
            else
                return O_RDWR;
            std::unreachable();
        }();

        const auto ret = ::open(path.c_str(), posix_access);
        if (ret == -1) return std::unexpected { error_code::from_errno() };
#endif
        m_descriptor = ret;
        return {};
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline auto readfile_to(const stdfs::path& path, array_view<meta::ModeToCharType<mode>> out) noexcept
      -> System_result<usize> {
        auto file = Try((TextFile<mode>::open(path, Access::READ)));
        ENSURES(stdr::size(out) >= file.size());
        Return Try(file.read_to(out));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline auto readfile(const stdfs::path& path) noexcept -> System_result<dynarray<meta::ModeToCharType<mode>>> {
        auto file = Try((TextFile<mode>::open(path, Access::READ)));
        auto out  = dynarray<meta::ModeToCharType<mode>> {};
        out.resize(file.size());
        auto readed = Try(file.read_to(out));
        out.resize(readed);
        Return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto readfile_to(const stdfs::path& path, array_view<byte> out) noexcept -> System_result<usize> {
        auto   file = Try(File::open(path, Access::READ));
        Return Try(file.read_to(out));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto readfile(const stdfs::path& path) noexcept -> System_result<dynarray<byte>> {
        auto file = Try((File::open(path, Access::READ)));
        auto out  = dynarray<byte> {};
        out.resize(file.size());
        auto readed = Try(file.read_to(out));
        out.resize(readed);
        Return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
        STORMKIT_FORCE_INLINE
    inline auto writefile(const stdfs::path& path, array_view<const meta::ModeToCharType<mode>> data) noexcept
      -> System_result<usize> {
        auto   file = Try((TextFile<mode>::open(path, Access::WRITE)));
        Return Try(file.write(data));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto writefile(const stdfs::path& path, array_view<byte> data) noexcept -> System_result<usize> {
        auto   file = Try((File::open(path, Access::WRITE)));
        Return Try(file.write(data));
    }
}}} // namespace stormkit::core::io
