// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <cstddef>
#include <cstdint>

namespace storm::core {
    using Byte   = std::byte;
    using UInt8  = std::uint8_t;
    using UInt16 = std::uint16_t;
    using UInt32 = std::uint32_t;
    using UInt64 = std::uint64_t;

    using Hash32 = UInt64;
    using Hash64 = UInt64;

    using Int8  = std::int8_t;
    using Int16 = std::int16_t;
    using Int32 = std::int32_t;
    using Int64 = std::int64_t;

    using ArraySize = std::size_t;
    using ByteCount = std::size_t;
    using Offset    = Int32;
    using UOffset   = UInt32;
} // namespace storm::core
