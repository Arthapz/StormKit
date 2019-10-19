#include <storm/graphics/framegraph/FrameGraph.hpp>
#include <storm/graphics/framegraph/FrameGraphBuilder.hpp>
#include <storm/graphics/framegraph/FramePass.hpp>

#include <stack>

#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/pipeline/Framebuffer.hpp>
#include <storm/render/pipeline/RenderPass.hpp>
#include <storm/render/resource/Texture.hpp>

using namespace storm;
using namespace storm::graphics;

////////////////////////////////////////
////////////////////////////////////////
FrameGraphBuilder::FrameGraphBuilder(const render::Device &device)
	: m_resources{device}, m_device{device} {
}

////////////////////////////////////////
////////////////////////////////////////
FrameGraphBuilder::~FrameGraphBuilder() = default;

////////////////////////////////////////
////////////////////////////////////////
FrameGraphBuilder::FrameGraphBuilder(FrameGraphBuilder &&) = default;

////////////////////////////////////////
////////////////////////////////////////
FrameGraphBuilder &FrameGraphBuilder::operator=(FrameGraphBuilder &&) = default;

////////////////////////////////////////
////////////////////////////////////////
FrameGraph FrameGraphBuilder::compile() {
	auto framegraph				 = FrameGraph{m_device};
	framegraph.m_resources		 = m_resources;
	framegraph.m_backbuffer_name = m_backbuffer_name;

	auto pass_ref_counts =
		std::unordered_map<FramePassBaseConstObserverPtr, std::uint32_t>{};
	pass_ref_counts.reserve(std::size(m_passes));

	// reference
	for (const auto &pass : m_passes) {
		auto pass_ptr = _std::make_observer(pass.get());
		pass_ptr->clear();

		FramePassBuilder builder{framegraph, *pass_ptr};
		pass->build(builder);

		pass_ref_counts.emplace(pass_ptr,
								std::size(pass->m_create_textures) +
									std::size(pass->m_write_textures));
	}

	const auto textures = framegraph.m_resources.textures();
	auto textures_ref_counts =
		std::unordered_map<FrameGraphTextureResourceConstObserverPtr,
						   std::uint32_t>{};
	textures_ref_counts.reserve(std::size(textures));
	auto unreferenced_resources = std::stack<FrameGraphTextureResourceCRef>{};

	for (const auto &texture : textures) {
		auto texture_ptr = _std::make_observer(texture.get());

		textures_ref_counts.emplace(texture_ptr, std::size(texture->m_readers));

		if (textures_ref_counts[texture_ptr] == 0 && texture->isTransient())
			unreferenced_resources.emplace(*texture);
	}

	// cull
	while (!unreferenced_resources.empty()) {
		auto &resource = unreferenced_resources.top().get();
		unreferenced_resources.pop();

		auto creator	= resource.m_creator;
		auto &ref_count = pass_ref_counts[creator];
		if (ref_count > 0)
			--ref_count;

		if (ref_count == 0 && !creator->cullImune())
			for (auto &iterator : creator->m_read_textures) {
				auto &read_resource	= iterator.get();
				auto read_resource_ptr = _std::make_observer(&read_resource);

				auto &read_ref_count = textures_ref_counts[read_resource_ptr];

				if (read_ref_count > 0)
					--read_ref_count;

				if (read_ref_count == 0)
					unreferenced_resources.emplace(read_resource);
			}

		for (auto &writer : resource.m_writers) {
			auto writer_ptr = _std::make_observer(&writer.get());

			auto &writer_ref_count = pass_ref_counts[writer_ptr];

			if (writer_ref_count > 0)
				--writer_ref_count;
			if (writer_ref_count == 0 && !writer.get().cullImune())
				for (auto &iterator : writer.get().m_read_textures) {
					auto &read_resource = iterator.get();
					auto read_resource_ptr =
						_std::make_observer(&read_resource);
					auto &read_ref_count =
						textures_ref_counts[read_resource_ptr];

					if (read_ref_count > 0)
						--read_ref_count;

					if (read_ref_count == 0)
						unreferenced_resources.emplace(read_resource);
				}
		}
	}

	for (auto &pass : m_passes) {
		auto pass_ptr   = _std::make_observer(pass.get());
		auto &ref_count = pass_ref_counts[pass_ptr];

		if (ref_count == 0 && !pass_ptr->cullImune())
			continue;

		auto realized_resources   = FrameGraph::Step::Resources{};
		auto derealized_resources = FrameGraph::Step::Resources{};

		for (auto &resource : pass->m_create_textures) {
			realized_resources.emplace_back(resource);
			if (std::empty(resource.get().m_readers) &&
				std::empty(resource.get().m_writers))
				derealized_resources.emplace_back(resource.get());
		}

		auto reads_writes = pass->m_read_textures;
		reads_writes.insert(std::end(reads_writes),
							std::begin(pass->m_write_textures),
							std::end(pass->m_write_textures));
		for (auto resource_ref : reads_writes) {
			auto &resource = resource_ref.get();

			if (!resource.isTransient())
				continue;

			auto valid		= false;
			auto last_index = std::ptrdiff_t{0};

			if (!std::empty(resource.m_readers)) {
				auto last_reader =
					std::find_if(std::cbegin(m_passes), std::cend(m_passes),
								 [&resource](const auto &iterator) {
									 return iterator.get() ==
											&resource.m_readers.back().get();
								 });
				if (last_reader != std::cend(m_passes)) {
					valid = true;
					last_index =
						std::distance(std::cbegin(m_passes), last_reader);
				}
			}

			if (!std::empty(resource.m_writers)) {
				auto last_writer =
					std::find_if(std::cbegin(m_passes), std::cend(m_passes),
								 [&resource](const auto &iterator) {
									 return iterator.get() ==
											&(resource.m_writers.back().get());
								 });
				if (last_writer != std::cend(m_passes)) {
					valid	  = true;
					last_index = std::max(
						last_index,
						std::distance(std::cbegin(m_passes), last_writer));
				}
			}

			if (valid &&
				m_passes[gsl::narrow_cast<std::size_t>(last_index)] == pass)
				derealized_resources.emplace_back(resource);
		}

		framegraph.m_steps.emplace_back(
			FrameGraph::Step{*pass.get(), std::move(realized_resources),
							 std::move(derealized_resources)});
	}

	return framegraph;
}
