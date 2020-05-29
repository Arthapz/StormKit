vulkan_dep  = dependency('vulkan')
gli_dep = dependency('gli')

vulkan_headers_dep = declare_dependency(dependencies: [vulkan_dep])

public_dependencies = [core_dep, log_dep, image_dep, window_dep, module_dep, gli_dep, vulkan_headers_dep]
private_dependencies = []

core_headers = files([
    'include/storm/render/core/vk_mem_alloc.h',
    'include/storm/render/core/Fwd.hpp',
    'include/storm/render/core/Vulkan.hpp',
    'include/storm/render/core/DynamicLoader.hpp',
    'include/storm/render/core/Enums.hpp',
    'include/storm/render/core/Instance.hpp',
    'include/storm/render/core/RenderCapabilities.hpp',
    'include/storm/render/core/MemoryBarrier.hpp',
    'include/storm/render/core/PhysicalDevice.hpp',
    'include/storm/render/core/PhysicalDeviceInfo.hpp',
    'include/storm/render/core/Device.hpp',
    'include/storm/render/core/QueueFamily.hpp',
    'include/storm/render/core/Surface.hpp',
    'include/storm/render/core/Queue.hpp',
    'include/storm/render/core/Command.hpp',
    'include/storm/render/core/CommandBuffer.hpp',
    'include/storm/render/core/Types.hpp',
])

core_inls = files([
    'include/storm/render/core/Enums.inl',
    'include/storm/render/core/Instance.inl',
    'include/storm/render/core/PhysicalDevice.inl',
    'include/storm/render/core/Device.inl',
    'include/storm/render/core/Surface.inl',
    'include/storm/render/core/Queue.inl',
    'include/storm/render/core/CommandBuffer.inl',
])

resource_headers = files([
    'include/storm/render/resource/Fwd.hpp',
    'include/storm/render/resource/Shader.hpp',
    'include/storm/render/resource/TextureSubresource.hpp',
    'include/storm/render/resource/TextureSubresourceRange.hpp',
    'include/storm/render/resource/TextureSubresourceLayers.hpp',
    'include/storm/render/resource/Texture.hpp',
    'include/storm/render/resource/TextureView.hpp',
    'include/storm/render/resource/Sampler.hpp',
    'include/storm/render/resource/HardwareBuffer.hpp',
])

resource_inls = files([
    'include/storm/render/resource/Shader.inl',
    'include/storm/render/resource/Texture.inl',
    'include/storm/render/resource/TextureView.inl',
    'include/storm/render/resource/Sampler.inl',
    'include/storm/render/resource/HardwareBuffer.inl',
])

pipeline_headers = files([
    'include/storm/render/pipeline/Fwd.hpp',
    'include/storm/render/pipeline/GraphicsPipeline.hpp',
    'include/storm/render/pipeline/GraphicsPipelineState.hpp',
    'include/storm/render/pipeline/RenderPass.hpp',
    'include/storm/render/pipeline/Framebuffer.hpp',
    'include/storm/render/pipeline/VertexBindingDescription.hpp',
    'include/storm/render/pipeline/VertexInputAttributeDescription.hpp',
    'include/storm/render/pipeline/DescriptorSetLayout.hpp',
    'include/storm/render/pipeline/DescriptorPool.hpp',
    'include/storm/render/pipeline/DescriptorSet.hpp',
    'include/storm/render/pipeline/PipelineCache.hpp',
])

pipeline_inls = files([
    'include/storm/render/pipeline/GraphicsPipeline.inl',
    'include/storm/render/pipeline/RenderPass.inl',
    'include/storm/render/pipeline/Framebuffer.inl',
    'include/storm/render/pipeline/DescriptorSetLayout.inl',
    'include/storm/render/pipeline/DescriptorPool.inl',
    'include/storm/render/pipeline/DescriptorSet.inl',
])

sync_headers = files([
    'include/storm/render/sync/Fwd.hpp',
    'include/storm/render/sync/Semaphore.hpp',
    'include/storm/render/sync/Fence.hpp',
])

sync_inls = files([
    'include/storm/render/sync/Semaphore.inl',
    'include/storm/render/sync/Fence.inl',
])

sources = files([
    'src/core/Instance.cpp',
    'src/core/DynamicLoader.cpp',
    'src/core/PhysicalDevice.cpp',
    'src/core/Device.cpp',
    'src/core/Surface.cpp',
    'src/core/Queue.cpp',
    'src/core/CommandBuffer.cpp',
    'src/core/Types.cpp',

    'src/resource/Shader.cpp',
    'src/resource/Texture.cpp',
    'src/resource/TextureView.cpp',
    'src/resource/ConvertFormatShaders.hpp',
    'src/resource/Sampler.cpp',
    'src/resource/HardwareBuffer.cpp',

    'src/pipeline/GraphicsPipeline.cpp',
    'src/pipeline/GraphicsPipelineState.cpp',
    'src/pipeline/RenderPass.cpp',
    'src/pipeline/Framebuffer.cpp',
    'src/pipeline/DescriptorSetLayout.cpp',
    'src/pipeline/DescriptorPool.cpp',
    'src/pipeline/DescriptorSet.cpp',
    'src/pipeline/VertexBindingDescription.cpp',
    'src/pipeline/VertexInputAttributeDescription.cpp',
    'src/pipeline/PipelineCache.cpp',

    'src/sync/Semaphore.cpp',
    'src/sync/Fence.cpp',
])

headers = core_headers + resource_headers + pipeline_headers + sync_headers

args = {}
args += {'extra_files': inls, 'gnu_symbol_visibility': 'hidden'}

public_render_inc = ['include/']
private_render_inc = []

args += {
    'dependencies': public_dependencies + private_dependencies,
    'include_directories': public_render_inc + private_render_inc
}

if lib_type == 'shared'
    render_lib_name = 'StormKit-render' + suffix
    render_lib = shared_library(
        render_lib_name,
        sources,
        headers,
        kwargs: args,
        install: true
    )
else
    render_lib_name = 'StormKit-render-static' + suffix
    render_lib = static_library(
        render_lib_name,
        sources,
        headers,
        kwargs: args,
        install: true
    )
endif

render_dep = declare_dependency(
    link_with: render_lib,
    dependencies: public_dependencies,
    include_directories: public_render_inc,
)

if get_option('enable_examples')
    subdir('samples')
endif

install_headers(core_headers + core_inls, subdir : 'storm/render/core')
install_headers(resource_headers + resource_inls, subdir : 'storm/render/resource')
install_headers(pipeline_headers + pipeline_inls, subdir : 'storm/render/pipeline')
install_headers(sync_headers + sync_inls, subdir : 'storm/render/sync')
