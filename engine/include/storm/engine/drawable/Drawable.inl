// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    const BoundingBox &Drawable::boundingBox() const noexcept {
        if (m_is_bounding_box_dirty) {
            recomputeBoundingBox();
            m_is_bounding_box_dirty = false;
        }

        return m_bounding_box;
    }
} // namespace storm::engine
