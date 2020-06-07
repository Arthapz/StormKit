// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    /////////////////////////////////////
    ////////////////////////////////////////
    [[nodiscard]] const Material &Mesh::material() const noexcept { return *m_material; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    [[nodiscard]] Transform &Mesh::transform() noexcept { return *m_transform; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    [[nodiscard]] const Transform &Mesh::transform() const noexcept { return *m_transform; }
} // namespace storm::engine
