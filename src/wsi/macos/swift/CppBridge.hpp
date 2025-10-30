#ifndef CPP_BINDINGS_HPP
#define CPP_BINDINGS_HPP

#include <cstdint>
#include <limits>

using ID = std::uint64_t;

extern "C" {
    enum class _Key : std::uint8_t {
        A = 0,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        NUM_0,
        NUM_1,
        NUM_2,
        NUM_3,
        NUM_4,
        NUM_5,
        NUM_6,
        NUM_7,
        NUM_8,
        NUM_9,
        ESCAPE,
        L_CONTROL,
        L_SHIFT,
        L_ALT,
        L_META,
        R_CONTROL,
        R_SHIFT,
        R_ALT,
        R_META,
        MENU,
        L_BRACKET,
        R_BRACKET,
        SEMI_COLON,
        COMMA,
        PERIOD,
        QUOTE,
        SLASH,
        BACK_SLASH,
        TILDE,
        EQUAL,
        HYPHEN,
        SPACE,
        ENTER,
        BACK_SPACE,
        TAB,
        PAGE_UP,
        PAGE_DOWN,
        BEGIN,
        END,
        HOME,
        INSERT,
        DELETE,
        ADD,
        SUBSTRACT,
        MULTIPLY,
        DIVIDE,
        LEFT,
        RIGHT,
        UP,
        DOWN,
        NUMPAD_0,
        NUMPAD_1,
        NUMPAD_2,
        NUMPAD_3,
        NUMPAD_4,
        NUMPAD_5,
        NUMPAD_6,
        NUMPAD_7,
        NUMPAD_8,
        NUMPAD_9,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,
        F14,
        F15,
        PAUSE,
        UNKNOWN = std::numeric_limits<std::uint8_t>::max(),
    };

    auto localizedKey(char code) noexcept -> std::uint8_t;
    auto usageToVirtualCode(std::int32_t usage) noexcept -> std::int32_t;

    auto swiftClosedEvent(ID) noexcept -> bool;
    auto swiftResizedEvent(ID, float, float) noexcept -> void;
    auto swiftRestoredEvent(ID) noexcept -> void;
    auto swiftMinimizedEvent(ID) noexcept -> void;
    auto swiftActivatedEvent(ID) noexcept -> void;
    auto swiftDeactivatedEvent(ID) noexcept -> void;

    auto swiftMouseDownEvent(ID, std::int32_t, std::int32_t, std::int32_t) noexcept -> void;
    auto swiftMouseUpEvent(ID, std::int32_t, std::int32_t, std::int32_t) noexcept -> void;
    auto swiftMouseMovedEvent(ID, std::int32_t, std::int32_t) noexcept -> void;

    auto swiftKeyDownEvent(ID, std::uint16_t, char) noexcept -> void;
    auto swiftKeyUpEvent(ID, std::uint16_t, char) noexcept -> void;
}

#endif
