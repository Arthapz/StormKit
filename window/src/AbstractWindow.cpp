/////////// - StormKit::window - ///////////
#include <storm/window/AbstractWindow.hpp>
#include <storm/window/Event.hpp>

using namespace storm;
using namespace storm::window;

/////////////////////////////////////
/////////////////////////////////////
AbstractWindow::AbstractWindow() = default;

/////////////////////////////////////
/////////////////////////////////////
AbstractWindow::~AbstractWindow() = default;

/////////////////////////////////////
/////////////////////////////////////
AbstractWindow::AbstractWindow(AbstractWindow &&) = default;

/////////////////////////////////////
/////////////////////////////////////
AbstractWindow &AbstractWindow::operator=(AbstractWindow &&) = default;

/////////////////////////////////////
/////////////////////////////////////
bool AbstractWindow::pollEvent(Event &event, [[maybe_unused]] void *native_event) noexcept {
    auto has_event = !m_events.empty();

    if (has_event) {
        event = m_events.front();
        m_events.pop();
    }

    return has_event;
}

/////////////////////////////////////
/////////////////////////////////////
bool AbstractWindow::waitEvent(Event &event, [[maybe_unused]] void *native_event) noexcept {
    auto has_event = !m_events.empty();

    if (has_event) {
        event = m_events.front();
        m_events.pop();
    }

    return has_event;
}

/////////////////////////////////////
/////////////////////////////////////
void AbstractWindow::mouseDownEvent(MouseButton button, core::Int16 x, core::Int16 y) noexcept {
    Event event;
    event.type               = EventType::MouseButtonPushed;
    event.mouse_event.button = button;
    event.mouse_event.x      = x;
    event.mouse_event.y      = y;

    pushEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
void AbstractWindow::mouseUpEvent(MouseButton button, core::Int16 x, core::Int16 y) noexcept {
    Event event;
    event.type               = EventType::MouseButtonReleased;
    event.mouse_event.button = button;
    event.mouse_event.x      = x;
    event.mouse_event.y      = y;

    pushEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
void AbstractWindow::mouseMoveEvent(core::Int16 x, core::Int16 y) noexcept {
    Event event;
    event.type          = EventType::MouseMoved;
    event.mouse_event.x = x;
    event.mouse_event.y = y; // Up Left 0/0

    pushEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
void AbstractWindow::mouseEnteredEvent() noexcept {
    Event event;
    event.type = EventType::MouseEntered;

    pushEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
void AbstractWindow::mouseExitedEvent() noexcept {
    Event event;
    event.type = EventType::MouseExited;

    pushEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
void AbstractWindow::pushEvent(Event event) noexcept {
    m_events.emplace(event);
}

/////////////////////////////////////
/////////////////////////////////////
void AbstractWindow::keyDownEvent(Key key) noexcept {
    Event event;
    event.type          = EventType::KeyPressed;
    event.key_event.key = key;

    pushEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
void AbstractWindow::keyUpEvent(Key key) noexcept {
    Event event;
    event.type          = EventType::KeyReleased;
    event.key_event.key = key;

    pushEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
void AbstractWindow::closeEvent() noexcept {
    Event event;
    event.type = EventType::Closed;

    pushEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
void AbstractWindow::resizeEvent(core::UInt16 width, core::UInt16 height) noexcept {
    m_video_settings.size.width  = width;
    m_video_settings.size.height = height;

    Event event;
    event.type                 = EventType::Resized;
    event.resized_event.width  = width;
    event.resized_event.height = height;

    pushEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
void AbstractWindow::minimizeEvent() noexcept {
    Event event;
    event.type = EventType::Minimized;

    pushEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
void AbstractWindow::maximizeEvent() noexcept {
    Event event;
    event.type = EventType::Maximized;

    pushEvent(event);
}
