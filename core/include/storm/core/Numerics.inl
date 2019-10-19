#pragma once

namespace storm::core {
    namespace private_ {
        static std::default_random_engine generator{};
    }

    inline void seed(std::uint32_t seed) noexcept { private_::generator.seed(seed); }

    template<typename T>
    T rand(T &&min, T &&max) noexcept {
        using Type = std::decay_t<std::remove_cv_t<T>>;
        if constexpr(std::is_floating_point_v<Type>) {
            std::uniform_real_distribution<Type> dis(min, max);
            return dis(private_::generator);
        } else {
            std::uniform_int_distribution<Type> dis(min, max);
            return dis(private_::generator);
        }
    }

    template<typename T, typename V, typename>
    inline constexpr auto realIsEqual(T &&a, V &&b) noexcept {
        return std::abs(a - b) < std::numeric_limits<std::remove_cv_t<std::decay_t<T>>>::epsilon();
    }

    template<typename T>
    inline constexpr T map(T x, T in_min, T in_max, T out_min, T out_max) noexcept {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    template<typename T>
    inline constexpr float degToRad(T angle) noexcept {
        return angle / 180. * pi<T>;
    }

    template<typename T>
    inline constexpr T radToDeg(T angle) noexcept {
        return angle / pi<T> * 180.;
    }

    template<class Integer, Integer start, Integer stop>
    constexpr auto genRange() noexcept {
        auto range = std::array<Integer, stop - start>{};
        std::iota(std::begin(range), std::end(range), start);
        return range;
    }

    template<class Integer, class Integer2>
    constexpr auto genRange(Integer start, Integer stop) noexcept {
        auto range = std::vector<Integer>{};
        range.resize(stop - start);
        std::iota(std::begin(range), std::end(range), start);
        return range;
    }

    template<class T>
    inline std::intptr_t extractOwnedPtr(T *ptr) noexcept {
        return reinterpret_cast<std::intptr_t>(ptr);
    }
} // namespace storm::core
