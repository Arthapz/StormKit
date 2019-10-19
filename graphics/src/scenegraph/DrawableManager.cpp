#include <storm/graphics/scenegraph/DrawableManager.hpp>

#include <storm/render/core/Device.hpp>

using namespace storm;
using namespace storm::graphics;

////////////////////////////////////////
////////////////////////////////////////
DrawableManager::DrawableManager(const render::Device &device,
								 std::uint32_t image_count)
	: m_device{device}, m_image_count{image_count} {
}

////////////////////////////////////////
////////////////////////////////////////
DrawableManager::~DrawableManager() = default;

////////////////////////////////////////
////////////////////////////////////////
DrawableManager::DrawableManager(DrawableManager &&) = default;

////////////////////////////////////////
////////////////////////////////////////
DrawableManager &DrawableManager::operator=(DrawableManager &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void DrawableManager::addDrawable(const render::GraphicsPipelineState &state,
								  const Material &material, DrawableData data) {
	Expects(!hasDrawable(state, material, data.node));

	auto &device = m_device.get();

	auto drawable = GPUDrawable{};
	drawable.node = data.node;

	std::visit(core::overload{[&drawable, &device, this](const auto &array) {
				   auto vertex_buffer_size =
					   std::size(array) *
					   sizeof(typename std::remove_reference_t<decltype(
								  array)>::value_type);
				   drawable.vertex_buffer = device.createVertexBuffer(
					   vertex_buffer_size * m_image_count,
					   render::MemoryProperty::Device_Local, true);
			   }},
			   data.vertex_array);

	std::visit(core::overload{
				   [&drawable, &device, this](const ShortIndexArray &array) {
					   auto index_buffer_size =
						   std::size(array) * sizeof(std::uint16_t);
					   drawable.index_buffer = device.createIndexBuffer(
						   index_buffer_size * m_image_count,
						   render::MemoryProperty::Device_Local, true);
				   },
				   [&drawable, &device, this](const LargeIndexArray &array) {
					   auto index_buffer_size =
						   std::size(array) * sizeof(std::uint32_t);
					   drawable.index_buffer = device.createIndexBuffer(
						   index_buffer_size * m_image_count,
						   render::MemoryProperty::Device_Local, true);
					   drawable.use_large_indices = true;
				   }},
			   data.index_array);

	drawable.per_drawable_buffer =
		device.createUniformBuffer(sizeof(render::Matrix) * 2 * m_image_count);

	auto &drawables = m_drawables[state][material];
	if (!hasMaterial(material))
		createMaterial(material);
	drawables.emplace_back(std::move(drawable));
}

void DrawableManager::updateDrawable(const render::GraphicsPipelineState &state,
									 const Material &material,
									 core::TreeNode::IndexType index,
									 UpdateData data) {
}

////////////////////////////////////////
////////////////////////////////////////
void DrawableManager::removeDrawable(const render::GraphicsPipelineState &state,
									 const Material &material,
									 ObjectTreeNode::IndexType index) {
}

////////////////////////////////////////
////////////////////////////////////////
void DrawableManager::createMaterial(const Material &material) {
	/*m_descriptor_pool = m_device->createDescriptorPool(
		*m_descriptor_set_layout,
	{{storm::render::DescriptorType::UniformBuffer, surface_image_count},
	{storm::render::DescriptorType::Combined_Image_Sampler,
	surface_image_count}}, surface_image_count);

	m_descriptor_sets =
	  m_descriptor_pool->allocateDescriptorSets(surface_image_count);
	m_gpu_materials[material].descriptor_sets =*/
}

////////////////////////////////////////
////////////////////////////////////////
DrawableManager::Drawables::iterator
	DrawableManager::getDrawable(const render::GraphicsPipelineState &state,
								 const Material &material,
								 ObjectTreeNode::IndexType index) {
	auto &drawables = m_drawables[state][material];

	return std::find_if(
		std::begin(drawables), std::end(drawables),
		[index](const auto &drawable) { return drawable.node == index; });
}
