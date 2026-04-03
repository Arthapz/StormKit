// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/flags_macro.hpp>
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

export module stormkit.core:utils.filesystem;

import std;

import :errors;

import :utils.contract;

import :typesafe;
import :functional;
import :meta;
import :named_constructors;
// import :containers;

namespace stdfs = std::filesystem;

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
    using ModeToCharType = core::meta::If<mode == Mode::WIDE, wchar, char>;
}}} // namespace stormkit::core::io::meta

export {
    namespace stormkit { inline namespace core {
        namespace io {
            template<typename T>
            using Expected = std::expected<T, SystemError>;

            enum class Access {
                READ,
                WRITE,
            };

            template<Mode mode>
            class Descriptor;
        } // namespace io

        namespace io {
            template<Mode mode>
            class Descriptor final: public NamedConstructor<Descriptor<mode>, DoInitArgs<const stdfs::path&, Access>> {
                using Base = NamedConstructor<Descriptor<mode>, DoInitArgs<const stdfs::path&, Access>>;

              public:
                explicit Descriptor(PrivateTag) noexcept;
                ~Descriptor() noexcept;

                Descriptor(Descriptor&)                    = delete;
                auto operator=(Descriptor&) -> Descriptor& = delete;

                Descriptor(Descriptor&&) noexcept;
                auto operator=(Descriptor&&) noexcept -> Descriptor&;

                static auto open(const stdfs::path& path, Access access) noexcept -> Expected<Descriptor>;
                static auto allocate_and_open(const stdfs::path& path, Access access) noexcept -> Expected<Descriptor>;

                auto close() noexcept;

                auto read_to(byte_mut_view<> out) noexcept -> Expected<usize>;
                auto read_to(array_view<char> out) noexcept -> Expected<usize>
                    requires(mode == Mode::UTF8 or mode == Mode::AINSI);
                auto read_to(array_view<wchar> out) noexcept -> Expected<usize>
                    requires(mode == Mode::WIDE);

                auto write(byte_view<> bytes) noexcept -> Expected<usize>;
                auto write(array_view<const char> bytes) noexcept -> Expected<usize>
                    requires(mode == Mode::UTF8 or mode == Mode::AINSI);
                auto write(array_view<const wchar> bytes) noexcept -> Expected<usize>
                    requires(mode == Mode::WIDE);

                auto flush() noexcept -> void;

                auto position() const noexcept -> usize;
                auto size() const noexcept -> usize;

                auto native_descriptor() const noexcept -> int;

                auto do_init(PrivateTag, const stdfs::path&, Access) noexcept -> Expected<void>;

              private:
                using Base::allocate;
                using Base::create;

                int m_descriptor = 0;

                mutable std::atomic<usize> m_size = 0;
            };

            using File = Descriptor<Mode::BINARY>;
            template<Mode mode = Mode::UTF8>
                requires(mode != Mode::BINARY)
            using TextFile = Descriptor<mode>;

            template<Mode mode = Mode::UTF8>
            auto read_text_to(const stdfs::path& path, array_view<meta::ModeToCharType<mode>> output) noexcept -> Expected<usize>;
            template<Mode mode = Mode::UTF8>
            auto read_text(const stdfs::path& path) noexcept -> Expected<dyn_array<meta::ModeToCharType<mode>>>;

            auto read_to(const stdfs::path& path, byte_mut_view<> output) noexcept -> Expected<usize>;
            auto read(const stdfs::path& path) noexcept -> Expected<byte_dyn_array>;

            template<Mode mode = Mode::UTF8>
            auto write_text(const stdfs::path& path, array_view<const meta::ModeToCharType<mode>> data) noexcept
              -> Expected<usize>;
            auto write(const stdfs::path& path, byte_view<> data) noexcept -> Expected<usize>;
        } // namespace io
    }} // namespace stormkit::core

    FLAG_ENUM(stormkit::core::io::Access);
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
    inline Descriptor<mode>::Descriptor(PrivateTag) noexcept {
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
        : m_descriptor { std::exchange(other.m_descriptor, 0) } {
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

        m_descriptor = std::exchange(other.m_descriptor, 0);
        m_size       = other.m_size.load();
        other.m_size.store(0);

        return *this;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline auto Descriptor<mode>::open(const stdfs::path& path, Access access) noexcept -> Expected<Descriptor<mode>> {
        return Descriptor<mode>::create(path, access);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline auto Descriptor<mode>::allocate_and_open(const stdfs::path& path, Access access) noexcept
      -> Expected<Descriptor<mode>> {
        return Descriptor<mode>::allocate(path, access);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline auto Descriptor<mode>::close() noexcept {
        if (m_descriptor != 0) {
            flush();
#ifdef STORMKIT_OS_WINDOWS
            _close
#else
            ::close
#endif
              (m_descriptor);
            m_descriptor = 0;
            m_size       = 0;
        }
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline auto Descriptor<mode>::read_to(byte_mut_view<> out) noexcept -> Expected<usize> {
        EXPECTS(m_descriptor != 0);
        const auto ret =
#ifdef STORMKIT_OS_WINDOWS
          _read
#else
          ::read
#endif
          (m_descriptor, std::bit_cast<void*>(stdr::data(out)), as<u32>(stdr::size(out)));

        if (ret == -1) return std::unexpected { SystemError::from_errno() };
        return Expected<usize> { std::in_place, as<usize>(ret) };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline auto Descriptor<mode>::read_to(array_view<char> out) noexcept -> Expected<usize>
        requires(mode == Mode::UTF8 or mode == Mode::AINSI)
    {
        return read_to(as_bytes_mut(out));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline auto Descriptor<mode>::read_to(array_view<wchar> out) noexcept -> Expected<usize>
        requires(mode == Mode::WIDE)
    {
        return read_to(as_bytes_mut(out));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline auto Descriptor<mode>::write(byte_view<> data) noexcept -> Expected<usize> {
        EXPECTS(m_descriptor != 0);
        const auto ret =
#ifdef STORMKIT_OS_WINDOWS
          _write
#else
          ::write
#endif
          (m_descriptor, std::bit_cast<const void*>(stdr::data(data)), as<u32>(stdr::size(data)));

        if (ret == -1) return std::unexpected { SystemError::from_errno() };
        return Expected<usize> { std::in_place, as<usize>(ret) };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline auto Descriptor<mode>::write(array_view<const char> data) noexcept -> Expected<usize>
        requires(mode == Mode::UTF8 or mode == Mode::AINSI)
    {
        return write(as_bytes(data));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline auto Descriptor<mode>::write(array_view<const wchar> data) noexcept -> Expected<usize>
        requires(mode == Mode::WIDE)
    {
        return write(as_bytes(data));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline auto Descriptor<mode>::flush() noexcept -> void {
        EXPECTS(m_descriptor != 0);
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
        EXPECTS(m_descriptor != 0);
        return as<usize>(
#ifdef STORMKIT_OS_WINDOWS
          _lseek
#else
          lseek
#endif
          (m_descriptor, 0, SEEK_CUR));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    inline auto Descriptor<mode>::size() const noexcept -> usize {
        EXPECTS(m_descriptor != 0);
        if (m_size == 0) {
            const auto old_pos = position();
#ifdef STORMKIT_OS_WINDOWS
            _lseek(m_descriptor, 0, SEEK_SET);
            m_size = as<usize>(_lseek(m_descriptor, 0, SEEK_END));
            _lseek(m_descriptor, as<i32>(old_pos), SEEK_SET);
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
    inline auto Descriptor<mode>::native_descriptor() const noexcept -> int {
        EXPECTS(m_descriptor != 0);
        return m_descriptor;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    inline auto Descriptor<mode>::do_init(PrivateTag, const stdfs::path& path, Access access) noexcept -> Expected<void> {
        if (access == Access::READ and not stdfs::exists(path))
            return std::unexpected { SystemError { .code = std::errc::no_such_file_or_directory } };
        if (stdfs::is_directory(path)) return std::unexpected { SystemError { .code = std::errc::is_a_directory } };

        const auto posix_access = [&access]() noexcept {
#ifdef STORMKIT_OS_WINDOWS
            if (access == Access::READ) return _O_RDONLY;
            else if (access == Access::WRITE)
                return (_O_WRONLY | _O_CREAT);
            else
                return _O_RDWR;
#else
            if (access == Access::READ) return O_RDONLY;
            else if (access == Access::WRITE)
                return O_WRONLY | O_CREAT;
            else
                return O_RDWR;
#endif
            std::unreachable();
        }();

#ifdef STORMKIT_OS_WINDOWS
        const auto text_mode = []() noexcept {
            switch (mode) {
    #ifdef STORMKIT_OS_WINDOWS
                case Mode::BINARY: return _O_BINARY;
                case Mode::AINSI: [[fallthrough]];
                case Mode::UTF8: return _O_TEXT;
                case Mode::WIDE: return _O_WTEXT;
                default: break;
    #else
                default: return 0;
    #endif
            };
            std::unreachable();
        }();

        auto ret = 0;
        auto str = path.string();
        const auto //
          err = _sopen_s(&ret, str.c_str(), posix_access | text_mode, _SH_DENYNO, _S_IREAD);
        if (err != 0) return std::unexpected { SystemError::from_errno() };
#else
        const auto ret = ::open(path.c_str(), posix_access);
        if (ret == -1) return std::unexpected { SystemError::from_errno() };
#endif
        m_descriptor = ret;
        return {};
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline auto read_text_to(const stdfs::path& path, array_view<meta::ModeToCharType<mode>> out) noexcept -> Expected<usize> {
        auto file = Try((TextFile<mode>::open(path, Access::READ)));
        ENSURES(stdr::size(out) >= file.size());
        Return Try(file.read_to(out));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
    STORMKIT_FORCE_INLINE
    inline auto read_text(const stdfs::path& path) noexcept -> Expected<dyn_array<meta::ModeToCharType<mode>>> {
        auto file = Try((TextFile<mode>::open(path, Access::READ)));
        auto out  = dyn_array<meta::ModeToCharType<mode>> {};
        out.resize(file.size());
        auto readed = Try(file.read_to(out));
        out.resize(readed);
        Return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto read_to(const stdfs::path& path, byte_mut_view<> out) noexcept -> Expected<usize> {
        auto   file = Try((File::open(path, Access::READ)));
        Return Try(file.read_to(out));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto read(const stdfs::path& path) noexcept -> Expected<byte_dyn_array> {
        auto file = Try((File::open(path, Access::READ)));
        auto out  = byte_dyn_array {};
        out.resize(file.size());
        auto readed = Try(file.read_to(out));
        out.resize(readed);
        Return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<Mode mode>
        STORMKIT_FORCE_INLINE
    inline auto write_text(const stdfs::path& path, array_view<const meta::ModeToCharType<mode>> data) noexcept
      -> Expected<usize> {
        auto   file = Try((TextFile<mode>::open(path, Access::WRITE)));
        Return Try(file.write(data));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto write(const stdfs::path& path, byte_view<> data) noexcept -> Expected<usize> {
        auto   file = Try((File::open(path, Access::WRITE)));
        Return Try(file.write(data));
    }
}}} // namespace stormkit::core::io
