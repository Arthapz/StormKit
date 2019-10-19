project(StormKit-Render VERSION 0.1.0 LANGUAGES C CXX)

message("-- -- Detecting Storm::Render Dependencies -- --")

find_dependency(Vulkan QUIET REQUIRED)
if(TARGET Vulkan::Vulkan)
    message("-- Vulkan Found")
else()
    message(FATAL_ERROR "Fatal: Missing Vulkan library")
endif()

add_library(VMA::VMA INTERFACE IMPORTED)
set_target_properties(VMA::VMA
   PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/../external/VulkanMemoryAllocator/src"
)

if(STORM_OS_WIN32)
   set(VK_COMPILE_DEFINITION "VK_USE_PLATFORM_WIN32_KHR")
elseif(STORM_OS_MACOS)
   set(VK_COMPILE_DEFINITION "VK_USE_PLATFORM_MACOS_MVK")
elseif(STORM_OS_LINUX)
   set(VK_COMPILE_DEFINITION "VK_USE_PLATFORM_XCB_KHR")
endif()

set(SOURCE_FILES 
    src/core/Instance.cpp
    src/core/PhysicalDevice.cpp
    src/core/Device.cpp
    src/core/Surface.cpp
    src/core/Queue.cpp
    src/core/CommandBuffer.cpp
    src/core/Types.cpp

    src/resource/Shader.cpp
    src/resource/Texture.cpp
    src/resource/Sampler.cpp
    src/resource/HardwareBuffer.cpp

    src/pipeline/GraphicsPipeline.cpp
    src/pipeline/GraphicsPipelineState.cpp
    src/pipeline/RenderPass.cpp
    src/pipeline/Framebuffer.cpp
    src/pipeline/DescriptorSetLayout.cpp
    src/pipeline/DescriptorPool.cpp
    src/pipeline/DescriptorSet.cpp
    src/pipeline/VertexBindingDescription.cpp
    src/pipeline/VertexInputAttributeDescription.cpp
    src/pipeline/PipelineCache.cpp

    src/sync/Semaphore.cpp
    src/sync/Fence.cpp
 )

set(HEADER_FILES
    include/storm/render/core/Fwd.hpp
    include/storm/render/core/Vulkan.hpp
    include/storm/render/core/Enums.hpp
    include/storm/render/core/Enums.inl
    include/storm/render/core/Instance.hpp
    include/storm/render/core/Instance.inl
    include/storm/render/core/RenderCapabilities.hpp
    include/storm/render/core/PhysicalDevice.hpp
    include/storm/render/core/PhysicalDevice.inl
    include/storm/render/core/PhysicalDeviceInfo.hpp
    include/storm/render/core/Device.hpp
    include/storm/render/core/Device.inl
    include/storm/render/core/QueueFamily.hpp
    include/storm/render/core/Surface.hpp
    include/storm/render/core/Queue.hpp
    include/storm/render/core/Command.hpp
    include/storm/render/core/CommandBuffer.hpp
    include/storm/render/core/CommandBuffer.inl
    include/storm/render/core/Types.hpp

    include/storm/render/resource/Fwd.hpp
    include/storm/render/resource/Shader.hpp
    include/storm/render/resource/Texture.hpp
    include/storm/render/resource/Sampler.hpp
    include/storm/render/resource/HardwareBuffer.hpp

    include/storm/render/pipeline/Fwd.hpp
    include/storm/render/pipeline/GraphicsPipeline.hpp
    include/storm/render/pipeline/GraphicsPipelineState.hpp
    include/storm/render/pipeline/RenderPass.hpp
    include/storm/render/pipeline/Framebuffer.hpp
    include/storm/render/pipeline/VertexBindingDescription.hpp
    include/storm/render/pipeline/VertexInputAttributeDescription.hpp
    include/storm/render/pipeline/DescriptorSetLayout.hpp
    include/storm/render/pipeline/DescriptorPool.hpp
    include/storm/render/pipeline/DescriptorSet.hpp
    include/storm/render/pipeline/PipelineCache.hpp

    include/storm/render/sync/Fwd.hpp
    include/storm/render/sync/Semaphore.hpp
    include/storm/render/sync/Fence.hpp
)

add_library(
    stormkit-render
    ${STORM_OUTPUT_TYPE}
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

target_link_libraries(
    stormkit-render
    PUBLIC
        GSL::GSL
        StormKit::core
        StormKit::log
        StormKit::window
        StormKit::image
        volk
        VMA::VMA
    PRIVATE
        glslang
        SPIRV
)

target_include_directories(
    stormkit-render
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${CMAKE_SOURCE_DIR}/external/volk
)

target_compile_definitions(
    stormkit-render
    PUBLIC
        GLM_FORCE_RADIANS
        GL_FORCE_DEPTH_ZERO_TO_ONE
        ${VK_COMPILE_DEFINITION}
        VK_NO_PROTOTYPES
    PRIVATE
        STORM_BUILD
)

add_library(StormKit::render ALIAS stormkit-render)

if(NOT STORM_OS_MACOS)
        set_target_properties(stormkit-render PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")
endif()
set_common_properties(stormkit-render)
enable_cpp17(stormkit-render ${STORM_COMPILER})
append_prefix_and_suffix(stormkit-render)
export_macos_framework("${HEADER_FILES}" render)

if(STORM_ENABLE_EXAMPLES)
    add_subdirectory("samples/Triangle")
    add_subdirectory("samples/UniformBuffer")
    #add_subdirectory("samples/05 - Texture")
    #add_subdirectory("samples/06 - DepthBuffer")
    #add_subdirectory("samples/07 - Obj")
endif()
