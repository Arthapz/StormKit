// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/platform_macro.hpp>
#include <stormkit/core/try_expected.hpp>

#include <stormkit/gpu/api.hpp>
#include <stormkit/gpu/vulkan.hpp>

export module stormkit.gpu.resource:shader;

import std;

import stormkit.core;
import stormkit.gpu.core;

namespace stdr = std::ranges;
namespace stdv = std::views;

namespace cmeta = stormkit::core::meta;

export namespace stormkit::gpu {
    class Shader;

    namespace view {
        class Shader;
    } // namespace view

    namespace meta {
        template<>
        struct ObjectInfo<Shader> {
            using Of          = Shader;
            using ElementType = VkShaderModule;
            using DeleterType = PFN_vkDestroyShaderModule VolkDeviceTable::*;
            using ViewType    = view::Shader;
            using OwnedBy     = Device;

            static constexpr auto DEBUG_TYPE = DebugObjectType::SHADER_MODULE;
        };
    } // namespace meta

    class STORMKIT_GPU_API Shader: public OwnedByDevice<Shader> {
      public:
        enum class Error {
            INVALID_SPIRV,
        };

        using LoadError = std::variant<core::SystemError, Result, Error>;
        template<typename T>
        using LoadExpected = std::expected<T, LoadError>;

        static auto load_from_file(view::Device device, const std::filesystem::path& filepath, ShaderStageFlag type) noexcept
          -> LoadExpected<Shader>;
        static auto load_from_bytes(view::Device device, std::span<const byte> data, ShaderStageFlag type) noexcept
          -> Expected<Shader>;
        static auto load_from_spirv(view::Device device, std::span<const SpirvID> data, ShaderStageFlag type) noexcept
          -> Expected<Shader>;

        static auto allocate_and_load_from_file(view::Device                 device,
                                                const std::filesystem::path& filepath,
                                                ShaderStageFlag              type) noexcept -> LoadExpected<Heap<Shader>>;
        static auto allocate_and_load_from_bytes(view::Device device, std::span<const byte> data, ShaderStageFlag type) noexcept
          -> Expected<Heap<Shader>>;
        static auto allocate_and_load_from_spirv(view::Device             device,
                                                 std::span<const SpirvID> data,
                                                 ShaderStageFlag          type) noexcept -> Expected<Heap<Shader>>;
        ~Shader();

        Shader(const Shader&)                    = delete;
        auto operator=(const Shader&) -> Shader& = delete;

        Shader(Shader&&) noexcept;
        auto operator=(Shader&&) noexcept -> Shader&;

        [[nodiscard]]
        auto type() const noexcept -> ShaderStageFlag;
        [[nodiscard]]
        auto source() const noexcept -> const std::vector<SpirvID>&;
        [[nodiscard]]
        auto source_as_bytes() const noexcept -> std::span<const byte>;

        // clang-format off
  // private:
        // clang-format on
        Shader(PrivateTag, view::Device) noexcept;
        auto do_init(PrivateTag, std::vector<SpirvID>, ShaderStageFlag) -> Expected<void>;

      private:
        auto reflect() noexcept -> void;

        ShaderStageFlag      m_type   = ShaderStageFlag::NONE;
        std::vector<SpirvID> m_source = {};
    };

    namespace view {
        class STORMKIT_GPU_API Shader: public view::DeviceObject<gpu::Shader> {
          public:
            using ObjectInfo  = typename meta::ObjectInfo<gpu::Shader>;
            using ElementType = ObjectInfo::ElementType;
            using ViewType    = ObjectInfo::ViewType;

            Shader(const gpu::Shader& of) noexcept;
            template<cmeta::IsContainerOrPointerOf<gpu::Shader> T>
            Shader(const T& of) noexcept;
            ~Shader() noexcept;

            Shader(const Shader&) noexcept;
            auto operator=(const Shader&) noexcept -> Shader&;

            Shader(Shader&&) noexcept;
            auto operator=(Shader&&) noexcept -> Shader&;

            [[nodiscard]]
            auto type() const noexcept -> ShaderStageFlag;

          private:
            ShaderStageFlag m_type = ShaderStageFlag::NONE;
        };
    } // namespace view
} // namespace stormkit::gpu

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    inline auto sys_to_load_error(SystemError&& error) noexcept -> Shader::LoadError {
        return Shader::LoadError { std::move(error) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    inline auto result_to_load_error(gpu::Result&& error) noexcept -> Shader::LoadError {
        return Shader::LoadError { std::move(error) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Shader::Shader(PrivateTag, view::Device device) noexcept
        : OwnedByDevice<Shader> { std::move(device), &VolkDeviceTable::vkDestroyShaderModule } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Shader::~Shader() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Shader::Shader(Shader&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Shader::operator=(Shader&&) noexcept -> Shader& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Shader::load_from_file(view::Device device, const std::filesystem::path& filepath, ShaderStageFlag type) noexcept
      -> LoadExpected<Shader> {
        expects(std::filesystem::is_regular_file(filepath), std::format("{} is not a file", filepath.string()));

        const auto data  = TryTransformError(io::read(filepath), sys_to_load_error);
        const auto spirv = std::vector<SpirvID> { std::from_range, bytes_as_span<SpirvID>(data) };
        Return     TryTransformError(create(std::move(device), std::move(spirv), type), result_to_load_error);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Shader::load_from_bytes(view::Device device, std::span<const byte> data, ShaderStageFlag type) noexcept
      -> Expected<Shader> {
        const auto spirv = std::vector<SpirvID> { std::from_range, bytes_as_span<SpirvID>(data) };
        return create(std::move(device), std::move(spirv), type);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Shader::load_from_spirv(view::Device device, std::span<const SpirvID> data, ShaderStageFlag type) noexcept
      -> Expected<Shader> {
        const auto spirv = std::vector<SpirvID> { std::from_range, data };
        return create(std::move(device), std::move(spirv), type);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Shader::allocate_and_load_from_file(view::Device                 device,
                                                    const std::filesystem::path& filepath,
                                                    ShaderStageFlag              type) noexcept -> LoadExpected<Heap<Shader>> {
        expects(std::filesystem::is_regular_file(filepath), std::format("{} is not a file", filepath.string()));

        const auto data  = TryTransformError(io::read(filepath), sys_to_load_error);
        const auto spirv = std::vector<SpirvID> { std::from_range, bytes_as_span<SpirvID>(data) };
        Return     TryTransformError(allocate(std::move(device), std::move(spirv), type), result_to_load_error);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Shader::allocate_and_load_from_bytes(view::Device          device,
                                                     std::span<const byte> data,
                                                     ShaderStageFlag       type) noexcept -> Expected<Heap<Shader>> {
        const auto spirv = std::vector<SpirvID> { std::from_range, bytes_as_span<SpirvID>(data) };
        return allocate(std::move(device), std::move(spirv), type);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Shader::allocate_and_load_from_spirv(view::Device             device,
                                                     std::span<const SpirvID> data,
                                                     ShaderStageFlag          type) noexcept -> Expected<Heap<Shader>> {
        const auto spirv = std::vector<SpirvID> { std::from_range, data };
        return allocate(std::move(device), std::move(spirv), type);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Shader::type() const noexcept -> ShaderStageFlag {
        return m_type;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Shader::source() const noexcept -> const std::vector<SpirvID>& {
        return m_source;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Shader::source_as_bytes() const noexcept -> std::span<const byte> {
        return as_bytes(m_source);
    }

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Shader::Shader(const gpu::Shader& of) noexcept
            : view::DeviceObject<gpu::Shader> { of }, m_type { of.type() } {
        }

        ///////////////////////////////////
        ///////////////////////////////////
        template<cmeta::IsContainerOrPointerOf<gpu::Shader> T>
        STORMKIT_FORCE_INLINE
        inline Shader::Shader(const T& of) noexcept
            : view::DeviceObject<gpu::Shader> { of }, m_type { of->type() } {
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Shader::~Shader() noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Shader::Shader(const Shader& other) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Shader::operator=(const Shader& other) noexcept -> Shader& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Shader::Shader(Shader&&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Shader::operator=(Shader&&) noexcept -> Shader& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Shader::type() const noexcept -> ShaderStageFlag {
            return m_type;
        }
    } // namespace view
} // namespace stormkit::gpu
