// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/log/Severity.hpp>

namespace storm::log {
    void colorifyBegin(Severity severity, bool to_stderr) noexcept;
    void colorifyEnd(bool to_stderr) noexcept;
} // namespace storm::log
