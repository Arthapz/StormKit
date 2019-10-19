#include "Archetypes.hpp"
#include "../Common.hpp"
#include "../Tilemap.hpp"
#include "CameraComponent.hpp"
#include "CustomShaderDataComponent.hpp"
#include "StaticGeometryComponent.hpp"
#include "TextureComponent.hpp"
#include "TransformComponent.hpp"

#include <storm/entities/EntityManager.hpp>

#include <storm/render/pipeline/DescriptorSet.hpp>
#include <storm/render/resource/Texture.hpp>

#include <storm/graphics/core/Mesh.hpp>
#include <storm/graphics/core/Vertex.hpp>

storm::graphics::VertexP2UV2Array makeSpriteGeometry(
	const storm::core::Extentf &size,
	const storm::core::Extentf max_uvs = storm::core::Extentf{{1.f}, {1.f}}) {
	return {
		Vertex{.position = {0.f, 0.f}, .uv = {0.f, 0.f}},
		Vertex{.position = {0.f, size.h}, .uv = {0.f, max_uvs.h}},
		Vertex{.position = {size.w, 0.f}, .uv = {max_uvs.w, 0.f}},
		Vertex{.position = {size.w, size.h}, .uv = {max_uvs.w, max_uvs.h}},
	};
}

storm::graphics::VertexP2UV2Array
	makeTilemapGeometry(const storm::core::Extent &size) {
	auto tilemap = storm::graphics::VertexP2UV2Array{};
	tilemap.reserve(size.w * size.h * 6);

	const auto uv = storm::core::Vector2{
		1.f / gsl::narrow_cast<float>(size.w),
		1.f / gsl::narrow_cast<float>(size.h),
	};

	for (auto i = 0u; i < size.h; ++i) {
		for (auto j = 0u; j < size.w; ++j) {
			const auto x = gsl::narrow_cast<float>(j);
			const auto y = gsl::narrow_cast<float>(i);

			const auto v1 =
				Vertex{.position = {x * TILE_EXTENT.w, y * TILE_EXTENT.h},
					   .uv		 = {uv.x * x, uv.y * y}};
			const auto v2 = Vertex{
				.position = {x * TILE_EXTENT.w, (y + 1.f) * TILE_EXTENT.h},
				.uv		  = {uv.x * x, uv.y * (y + 1.f)}};
			const auto v3 = Vertex{
				.position = {(x + 1.f) * TILE_EXTENT.w, y * TILE_EXTENT.h},
				.uv		  = {uv.x * (x + 1.f), uv.y * y}};
			const auto v4 = Vertex{.position = {(x + 1.f) * TILE_EXTENT.w,
												(y + 1.f) * TILE_EXTENT.h},
								   .uv = {uv.x * (x + 1.f), uv.y * (y + 1.f)}};

			tilemap.emplace_back(v1);
			tilemap.emplace_back(v2);
			tilemap.emplace_back(v3);
			tilemap.emplace_back(v3);
			tilemap.emplace_back(v4);
			tilemap.emplace_back(v2);
		}
	}

	return tilemap;
}

storm::entities::Entity makeCamera(storm::entities::EntityManager &manager,
								   storm::core::Extentf size) {
	const auto projection_matrix = storm::core::ortho(0.f, size.w, 0.f, size.h);
	const auto view_matrix		 = storm::core::Matrix{1.f};

	auto entity = manager.makeEntity();

	auto &camera_component = manager.addComponent<CameraComponent>(entity);
	camera_component.projection_matrix = std::move(projection_matrix);
	camera_component.view_matrix	   = std::move(view_matrix);

	return entity;
}

storm::entities::Entity
	makeSprite(const storm::render::Device &device,
			   const storm::render::GraphicsPipelineState &pipeline,
			   storm::entities::EntityManager &manager,
			   storm::core::Extentf size, const storm::render::Texture &texture,
			   const storm::render::Sampler &sampler) {
	const auto geometry = makeSpriteGeometry(size, {{0.0588f}, {0.125f}});
	const auto indices  = storm::graphics::ShortIndexArray{0, 1, 2, 2, 3, 1};

	auto entity = manager.makeEntity();

	auto &geometry_component =
		manager.addComponent<StaticGeometryComponent>(entity);
	geometry_component.state		= storm::core::makeConstObserver(&pipeline);
	geometry_component.vertices		= std::move(geometry);
	geometry_component.vertex_count = 4;
	geometry_component.indices		= std::move(indices);
	geometry_component.index_count  = 6;

	auto &texture_component   = manager.addComponent<TextureComponent>(entity);
	texture_component.texture = storm::core::makeConstObserver(&texture);
	texture_component.sampler = storm::core::makeConstObserver(&sampler);

    auto &transform_component =
        manager.addComponent<TransformComponent>(entity);
    /*transform_component.transform_matrix =
        storm::core::translate(storm::core::Matrix{1.f}, {0.f, 0.f, -1.f});*/

	return entity;
}

storm::entities::Entity
	makeMap(const storm::render::Device &device,
			const storm::render::GraphicsPipelineState &pipeline,
			storm::entities::EntityManager &manager, storm::core::Extent size,
			const storm::render::Texture &texture,
			const storm::render::Sampler &sampler,
			const storm::render::DescriptorSet &set) {
	// const auto geometry = makeTilemapGeometry(size);
	const auto geometry =
		makeSpriteGeometry({.width  = {size.w * TILE_EXTENT.w},
                            .height = {size.h * TILE_EXTENT.h}});
	const auto indices = storm::graphics::ShortIndexArray{0, 1, 2, 2, 3, 1};

	auto entity = manager.makeEntity();

	auto &geometry_component =
		manager.addComponent<StaticGeometryComponent>(entity);
	geometry_component.state		= storm::core::makeConstObserver(&pipeline);
	geometry_component.vertex_count = std::size(geometry);
	geometry_component.vertices		= std::move(geometry);
	geometry_component.index_count  = std::size(indices);
	geometry_component.indices		= std::move(indices);

	auto &texture_component   = manager.addComponent<TextureComponent>(entity);
	texture_component.texture = storm::core::makeConstObserver(&texture);
	texture_component.sampler = storm::core::makeConstObserver(&sampler);

	auto &custom_shader_data =
		manager.addComponent<CustomShaderDataComponent>(entity);
	custom_shader_data.sets.emplace_back(storm::core::makeConstObserver(&set));

	return entity;
}
