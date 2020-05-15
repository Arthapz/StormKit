// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Fwd.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/framegraph/FramePassResource.hpp>
#include <storm/engine/framegraph/Fwd.hpp>

namespace storm::engine {
    class STORM_PUBLIC FramePassResourceHandle {
      public:
        FramePassResourceHandle(const FramePassResourceHandle &);
        FramePassResourceHandle &operator=(const FramePassResourceHandle &);

        FramePassResourceHandle(FramePassResourceHandle &&);
        FramePassResourceHandle &operator=(FramePassResourceHandle &&);

        ~FramePassResourceHandle();

        [[nodiscard]] inline core::UInt32 index() const noexcept;

        [[nodiscard]] inline bool isValid() const noexcept;
        inline void invalidate() noexcept;

        [[nodiscard]] inline bool operator<(const FramePassResourceHandle &rhs) const noexcept;
        [[nodiscard]] inline bool operator==(const FramePassResourceHandle &rhs) const noexcept;
        [[nodiscard]] inline bool operator!=(const FramePassResourceHandle &rhs) const noexcept;

      protected:
        friend class FrameGraph;
        template<typename T>
        friend class FramePassResourceID;

        static constexpr const auto INVALID = std::numeric_limits<core::UInt32>::max();

        FramePassResourceHandle() noexcept;
        explicit FramePassResourceHandle(core::UInt32 index) noexcept;

        core::UInt32 m_index = INVALID;
    };

    template<typename Resource>
    class STORM_PUBLIC FramePassResourceID: public FramePassResourceHandle {
      public:
        using FramePassResourceHandle::FramePassResourceHandle;

        FramePassResourceID() noexcept;
        explicit FramePassResourceID(FramePassResourceHandle handle) noexcept;

        FramePassResourceID(const FramePassResourceID &);
        FramePassResourceID &operator=(const FramePassResourceID &);

        FramePassResourceID(FramePassResourceID &&);
        FramePassResourceID &operator=(FramePassResourceID &&);

        ~FramePassResourceID();
    };
} // namespace storm::engine

HASH_FUNC(storm::engine::FramePassResourceHandle)
namespace std {
    template<typename Resource>
    struct hash<storm::engine::FramePassResourceID<Resource>> {
        [[nodiscard]] storm::core::Hash64
            operator()(const storm::engine::FramePassResourceID<Resource> &o) const noexcept {
            return std::hash<storm::engine::FramePassResourceHandle> {}(o);
        }
    };
    template<typename Resource>
    struct equal_to<storm::engine::FramePassResourceID<Resource>> {
        [[nodiscard]] inline bool
            operator()(const storm::engine::FramePassResourceID<Resource> &first,
                       const storm::engine::FramePassResourceID<Resource> &second) const noexcept {
            static auto hasher = hash<storm::engine::FramePassResourceID<Resource>> {};
            return hasher(first) == hasher(second);
        }
    };
} // namespace std

#include "FramePassResourceHandle.inl"
