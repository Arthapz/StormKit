// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Hash.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/drawable/3D/SubMesh.hpp>
#include <storm/engine/drawable/Drawable.hpp>

namespace storm::engine {
    class MeshTag;
    class STORM_PUBLIC Mesh final: public Drawable, public StaticBindable<MeshTag> {
      public:
        static constexpr auto MAX_JOINT_COUNT = 128;

        struct Node {
            using ID                      = core::UInt32;
            static constexpr auto INVALID = std::numeric_limits<ID>::max();
            static constexpr auto NO_SKIN = std::numeric_limits<core::UInt32>::max();

            ID parent = INVALID;
            std::vector<ID> childs;

            std::optional<SubMesh> submesh = std::nullopt;

            core::Vector3f translation = {};
            core::Vector3f scale       = core::Vector3f { 1.f };
            core::Quaternionf rotation = {};
            core::Matrixf matrix       = core::Matrixf { 1.f };
            core::UInt32 skin          = NO_SKIN;

            bool is_joint = false;

            inline core::Matrixf localTransform() const noexcept {
                auto transform = core::translate(core::Matrixf { 1.f }, translation) *
                                 core::Matrixf { rotation } *
                                 core::scale(core::Matrixf { 1.f }, scale) * matrix;

                return transform;
            }

            inline core::Matrixf globalTransform(core::span<const Node> nodes) const noexcept {
                auto transform = localTransform();

                auto parent_id = parent;
                while (parent_id != Node::NO_SKIN) {
                    const auto &parent = nodes[parent_id];

                    transform = parent.localTransform() * transform;

                    parent_id = parent.parent;
                }

                return transform;
            }
        };

        struct SkinTag;
        struct Skin {
            std::string name;

            Node::ID root_joint = Node::INVALID;

            std::vector<core::Matrixf> inverse_bind_matrices;

            std::vector<Node::ID> joints;
        };

        enum class InterpolationType { Linear, Step, Cubic_Spline };

        struct AnimationSampler {
            InterpolationType interpolation;
            std::vector<float> inputs;
            std::vector<core::Vector4f> outputs;
        };

        enum class Path { Translation, Rotation, Scale, Weights };

        struct AnimationChannel {
            Path path;
            Node::ID node;
            core::UInt32 sampler;
        };

        struct Animation {
            using Seconds = std::chrono::duration<float>;

            std::string name;
            std::vector<AnimationSampler> samplers;
            std::vector<AnimationChannel> channels;

            Seconds start        = Seconds { std::numeric_limits<float>::max() };
            Seconds end          = Seconds { std::numeric_limits<float>::min() };
            Seconds current_time = Seconds { 0.f };
        };

        Mesh(Engine &engine, const Material &material, std::string name = "");
        ~Mesh() override;

        Mesh(Mesh &&);
        Mesh &operator=(Mesh &&);

        core::UInt32 addSkin(Skin &&skin);
        Node::ID addNode(Node &&node);
        const Node &getNode(Node::ID id) const noexcept;

        [[nodiscard]] inline const Material &material() const noexcept;

        [[nodiscard]] inline Transform &transform() noexcept;
        [[nodiscard]] inline const Transform &transform() const noexcept;

        void render(render::CommandBuffer &cmb,
                    const render::RenderPass &pass,
                    std::vector<BindableBaseConstObserverPtr> bindables,
                    render::GraphicsPipelineState state,
                    float delta_time = 0.f) override;

        [[nodiscard]] inline std::string_view name() const noexcept;

        void bake();

        Mesh clone() const;
        MeshOwnedPtr clonePtr() const;

        void addAnimation(Animation &&animation);

        void playAnimation(core::UInt32 id, bool loop = false);
        void playAnimation(std::string_view name, bool loop = false);

      protected:
        void recomputeBoundingBox() const noexcept override;

      private:
        void updateAnimation(float delta_time);
        void updateNodes();

        struct alignas(16) Data {
            core::Matrixf transform = core::Matrixf { 1.f };
            std::array<core::Matrixf, MAX_JOINT_COUNT> joints;
            float joint_count = 0.f;
        };

        std::string m_name;

        MaterialConstObserverPtr m_material;
        TransformOwnedPtr m_transform;

        render::HardwareBufferOwnedPtr m_vertex_buffer;
        render::HardwareBufferOwnedPtr m_index_buffer;

        std::vector<Node> m_nodes;
        std::vector<Node::ID> m_root_nodes;
        std::vector<Node::ID> m_submesh_nodes;

        std::vector<Skin> m_skins;
        std::vector<Animation> m_animations;

        bool m_loop_animation                             = false;
        _std::observer_ptr<Animation> m_current_animation = nullptr;

        bool m_large_indices = false;

        bool m_need_to_update_nodes;
        RingHardwareBufferOwnedPtr m_mesh_data_buffer;
    };
} // namespace storm::engine

#include "Mesh.inl"
