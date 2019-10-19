#include <storm/window/AbstractWindow.hpp>
#include <storm/window/Event.hpp>

using namespace storm::window;

AbstractWindow::AbstractWindow()  = default;
AbstractWindow::~AbstractWindow() = default;

/////////////////////////////////////
/////////////////////////////////////
bool AbstractWindow::pollEvent(Event &event,
							   [[maybe_unused]] void *native_event) noexcept {
	auto has_event = !m_events.empty();

	if (has_event) {
		event = m_events.front();
		m_events.pop();
	}

	return has_event;
}

/////////////////////////////////////
/////////////////////////////////////
bool AbstractWindow::waitEvent(Event &event,
							   [[maybe_unused]] void *native_event) noexcept {
	auto has_event = !m_events.empty();

	if (has_event) {
		event = m_events.front();
		m_events.pop();
	}

	return has_event;
}

/////////////////////////////////////
/////////////////////////////////////
void AbstractWindow::mouseDownEvent(MouseButton button, std::int16_t x,
									std::int16_t y) noexcept {
	Event event;
	event.type				= EventType::MouseButtonPushed;
	event.mouseEvent.button = button;
	event.mouseEvent.x		= x;
	event.mouseEvent.y		= y;

	pushEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
void AbstractWindow::mouseUpEvent(MouseButton button, std::int16_t x,
								  std::int16_t y) noexcept {
	Event event;
	event.type				= EventType::MouseButtonReleased;
	event.mouseEvent.button = button;
	event.mouseEvent.x		= x;
	event.mouseEvent.y		= y;

	pushEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
void AbstractWindow::mouseMoveEvent(std::int16_t x, std::int16_t y) noexcept {
	Event event;
	event.type		   = EventType::MouseMoved;
	event.mouseEvent.x = x;
	event.mouseEvent.y = y; // Up Left 0/0

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
	event.type		   = EventType::KeyPressed;
	event.keyEvent.key = key;

	pushEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
void AbstractWindow::keyUpEvent(Key key) noexcept {
	Event event;
	event.type		   = EventType::KeyReleased;
	event.keyEvent.key = key;

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
void AbstractWindow::resizeEvent(std::uint16_t width,
								 std::uint16_t height) noexcept {
	m_video_settings.size.width  = width;
	m_video_settings.size.height = height;

	Event event;
	event.type				  = EventType::Resized;
	event.resizedEvent.width  = width;
	event.resizedEvent.height = height;

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
