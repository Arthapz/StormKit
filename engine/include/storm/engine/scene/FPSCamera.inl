// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    inline void FPSCamera::feedInputs(FPSCamera::Inputs inputs) { m_inputs = std::move(inputs); }
} // namespace storm::engine