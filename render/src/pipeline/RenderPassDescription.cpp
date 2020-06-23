#include <storm/render/pipeline/RenderPassDescription.hpp>

using namespace storm;
using namespace storm::render;

#define CHECK(x) if(!(x)) return false

/////////////////////////////////////
/////////////////////////////////////
bool RenderPassDescription::isCompatible(const RenderPassDescription &description) const noexcept {
    if(std::size(description.subpasses) != std::size(subpasses)) return false;
    for(auto  i = 0u;i < std::size(subpasses); ++i) {
        const auto &subpass_1 = subpasses[i];
        const auto &subpass_2 = description.subpasses[i];

        CHECK(subpass_1.bind_point == subpass_2.bind_point);

        const auto attachment_refs_count =
                std::min(std::size(subpass_1.attachment_refs), std::size(subpass_2.attachment_refs));

        for(auto j = 0u;j < attachment_refs_count; ++j) {
            const auto &attachment_ref_1 = subpass_1.attachment_refs[j];
            const auto &attachment_ref_2 = subpass_2.attachment_refs[j];

            CHECK(attachment_ref_1.layout == attachment_ref_2.layout);

            const auto &attachment_1 = description.attachments[attachment_ref_1.attachment_id];
            const auto &attachment_2 = description.attachments[attachment_ref_2.attachment_id];

            CHECK(attachment_1.format == attachment_2.format);
            CHECK(attachment_1.samples == attachment_2.samples);
        }
    }

    return true;
}

#undef CHECK

namespace std {
core::Hash64 hash<AttachmentDescription>::operator()(
    const AttachmentDescription &description) const noexcept {
    auto hash = core::Hash64 { 0 };

    core::hash_combine(hash, description.format);
    core::hash_combine(hash, description.samples);
    core::hash_combine(hash, description.load_op);
    core::hash_combine(hash, description.store_op);
    core::hash_combine(hash, description.stencil_load_op);
    core::hash_combine(hash, description.stencil_store_op);
    core::hash_combine(hash, description.source_layout);
    core::hash_combine(hash, description.destination_layout);
    core::hash_combine(hash, description.resolve);

    return hash;
}

core::Hash64 hash<Subpass>::operator()(
    const Subpass &description) const noexcept {
    auto hash = core::Hash64 { 0 };

    core::hash_combine(hash, description.bind_point);

    for(const auto &ref : description.attachment_refs)
        core::hash_combine(hash, ref);

    return hash;
}

core::Hash64 hash<Subpass::Ref>::operator()(
    const Subpass::Ref &description) const noexcept {
    auto hash = core::Hash64 { 0 };

    core::hash_combine(hash, description.attachment_id);
    core::hash_combine(hash, description.layout);

    return hash;
}

    core::Hash64 hash<RenderPassDescription>::operator()(
        const RenderPassDescription &description) const noexcept {
        auto hash = core::Hash64 { 0 };

        for(const auto &attachment : description.attachments)
            core::hash_combine(hash, attachment);

        for(const auto &subpass : description.subpasses)
            core::hash_combine(hash, subpass);

        return hash;
    }
}
