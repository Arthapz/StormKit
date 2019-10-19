// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>
#include <storm/render/pipeline/DescriptorPool.hpp>
#include <storm/render/pipeline/DescriptorSet.hpp>
#include <storm/render/resource/HardwareBuffer.hpp>
#include <storm/render/resource/Sampler.hpp>
#include <storm/render/resource/Texture.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
DescriptorSet::DescriptorSet(const DescriptorPool &pool,
                             std::vector<DescriptorType> types,
                             RAIIVkDescriptorSet set)
    : m_device { &pool.device() }, m_pool { &pool }, m_types { std::move(types) },
      m_vk_descriptor_set { std::move(set) } {
}

/////////////////////////////////////
/////////////////////////////////////
DescriptorSet::~DescriptorSet() = default;

/////////////////////////////////////
/////////////////////////////////////
DescriptorSet::DescriptorSet(DescriptorSet &&) = default;

/////////////////////////////////////
/////////////////////////////////////
DescriptorSet &DescriptorSet::operator=(DescriptorSet &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void DescriptorSet::update(core::span<const render::Descriptor> descriptors) {
    STORM_EXPECTS(!std::empty(descriptors));

    auto vk_buffer_infos =
        std::vector<std::tuple<core::UInt32, vk::DescriptorBufferInfo, vk::DescriptorType>> {};
    auto vk_image_infos =
        std::vector<std::tuple<core::UInt32, vk::DescriptorImageInfo, vk::DescriptorType>> {};

    for (const auto &descriptor_proxy : descriptors) {
        if (std::holds_alternative<BufferDescriptor>(descriptor_proxy)) {
            const auto &descriptor = std::get<BufferDescriptor>(descriptor_proxy);

            const auto buffer_info = vk::DescriptorBufferInfo {}
                                         .setBuffer(*descriptor.buffer)
                                         .setOffset(descriptor.offset)
                                         .setRange(descriptor.range);

            vk_buffer_infos.emplace_back(descriptor.binding,
                                         std::move(buffer_info),
                                         toVK(descriptor.type));
        } else if (std::holds_alternative<TextureDescriptor>(descriptor_proxy)) {
            const auto &descriptor = std::get<TextureDescriptor>(descriptor_proxy);

            auto image_info = vk::DescriptorImageInfo {}
                                  .setImageView(*descriptor.texture_view)
                                  .setImageLayout(toVK(descriptor.layout));

            if (descriptor.sampler != nullptr) image_info.setSampler(*descriptor.sampler);

            vk_image_infos.emplace_back(descriptor.binding,
                                        std::move(image_info),
                                        toVK(descriptor.type));
        }
    }

    auto vk_write_infos = std::vector<vk::WriteDescriptorSet> {};
    vk_write_infos.reserve(std::size(vk_buffer_infos) + std::size(vk_image_infos));

    for (const auto &[binding, vk_buffer_info, type] : vk_buffer_infos) {
        const auto write_info = vk::WriteDescriptorSet {}
                                    .setDstSet(*m_vk_descriptor_set)
                                    .setDstBinding(binding)
                                    .setDstArrayElement(0)
                                    .setDescriptorCount(1)
                                    .setDescriptorType(type)
                                    .setPBufferInfo(&vk_buffer_info);

        vk_write_infos.emplace_back(std::move(write_info));
    }

    for (const auto &[binding, vk_image_info, type] : vk_image_infos) {
        const auto write_info = vk::WriteDescriptorSet {}
                                    .setDstSet(*m_vk_descriptor_set)
                                    .setDstBinding(binding)
                                    .setDstArrayElement(0)
                                    .setDescriptorCount(1)
                                    .setDescriptorType(type)
                                    .setPImageInfo(&vk_image_info);

        vk_write_infos.emplace_back(std::move(write_info));
    }

    m_device->updateVkDescriptorSets(vk_write_infos, {});
}
