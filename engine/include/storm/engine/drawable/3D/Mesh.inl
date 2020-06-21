// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    const Material &Mesh::material() const noexcept { return *m_material; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    Transform &Mesh::transform() noexcept { return *m_transform; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    const Transform &Mesh::transform() const noexcept { return *m_transform; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    std::string_view Mesh::name() const noexcept { return m_name; }
} // namespace storm::engine