// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <filesystem>

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

namespace storm::engine {
    class STORMKIT_PUBLIC Model: public core::NonCopyable {
      public:
        explicit Model(Engine &engine);
        ~Model();

        Model(Model &&) noexcept;
        Model &operator=(Model &&) noexcept;

        void load(std::filesystem::path path);

        [[nodiscard]] PbrMeshOwnedPtr createMesh() const noexcept;
        [[nodiscard]] PbrMaterialOwnedPtr createMaterial() const noexcept;

      private:
        EngineRef m_engine;

        std::filesystem::path m_path;
    };
} // namespace storm::engine
