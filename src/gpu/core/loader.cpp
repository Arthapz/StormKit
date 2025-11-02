module;

#include <stormkit/gpu/vulkan.hpp>

module stormkit.gpu.core;

namespace stormkit::gpu {
    auto initialize_backend() -> Expected<void> {
        return vk_call(volkInitialize).transform_error(monadic::from_vk<Result>());
    }
} // namespace stormkit::gpu
