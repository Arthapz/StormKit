#include <storm/window/Event.hpp>
#include <storm/window/EventHandler.hpp>
#include <storm/window/Window.hpp>

using namespace storm;
using namespace storm::window;

/////////////////////////////////////
/////////////////////////////////////
EventHandler::EventHandler(Window &window) : m_window { &window } {
}

/////////////////////////////////////
/////////////////////////////////////
EventHandler::~EventHandler() = default;

/////////////////////////////////////
/////////////////////////////////////
EventHandler::EventHandler(const EventHandler &) = default;

/////////////////////////////////////
/////////////////////////////////////
EventHandler &EventHandler::operator=(const EventHandler &) = default;

/////////////////////////////////////
/////////////////////////////////////
EventHandler::EventHandler(EventHandler &&) = default;

/////////////////////////////////////
/////////////////////////////////////
EventHandler &EventHandler::operator=(EventHandler &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void EventHandler::update() {
    auto event = Event {};

    while (m_window->pollEvent(event)) {
        auto event_type = event.type;

        for (auto &callback : m_callback[event_type]) callback(event);
    }
}
