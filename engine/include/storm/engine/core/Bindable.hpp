// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Assert.hpp>
#include <storm/core/Hash.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

#include <storm/render/pipeline/DescriptorSetLayout.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

namespace storm::engine {
    class STORM_PUBLIC BindableBase: public core::NonCopyable {
      public:
        BindableBase();
        virtual ~BindableBase();

        BindableBase(BindableBase &&);
        BindableBase &operator=(BindableBase &&);

        [[nodiscard]] virtual const render::DescriptorSetLayout &
            descriptorLayout() const noexcept = 0;
        [[nodiscard]] inline const render::DescriptorSet &descriptorSet() const noexcept;
        [[nodiscard]] inline const std::optional<core::UOffset> &offset() const noexcept;

      protected:
        [[nodiscard]] inline render::DescriptorSet &descriptorSet() noexcept;
        inline void next() noexcept;

        core::ArraySize m_current_descriptor_set = 0;
        render::DescriptorSetOwnedPtrArray m_descriptor_sets;
        std::optional<core::UOffset> m_offset = std::nullopt;
    };

    template<typename T>
    class STORM_PUBLIC StaticBindable: public BindableBase {
      public:
        using Tag = T;
        using InitFunc =
            std::function<void(const render::Device &, render::DescriptorSetLayoutOwnedPtr &)>;
        StaticBindable();
        ~StaticBindable() override;

        StaticBindable(StaticBindable &&);
        StaticBindable &operator=(StaticBindable &&);

        static void initDescriptorLayout(const render::Device &device, const InitFunc &func);
        static void destroyDescriptorLayout();

        [[nodiscard]] inline const render::DescriptorSetLayout &
            descriptorLayout() const noexcept final;

      private:
        static inline render::DescriptorSetLayoutOwnedPtr s_descriptor_set_layout;
    };

    class STORM_PUBLIC Bindable: public BindableBase {
      public:
        using DescriptorSetVariant = std::variant<render::DescriptorSetLayoutOwnedPtr,
                                                  render::DescriptorSetLayoutConstObserverPtr>;

        using InitFunc = std::function<void(DescriptorSetVariant &)>;
        explicit Bindable(InitFunc doInitDescriptorSetLayout);
        ~Bindable() override;

        Bindable(Bindable &&);
        Bindable &operator=(Bindable &&);

        [[nodiscard]] inline const render::DescriptorSetLayout &
            descriptorLayout() const noexcept final;

      private:
        InitFunc m_doInitDescriptorSetLayout;

        DescriptorSetVariant m_descriptor_set_layout;
    };
} // namespace storm::engine

#include "Bindable.inl"
