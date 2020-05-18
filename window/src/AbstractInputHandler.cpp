/////////// - StormKit::window - ///////////
#include <storm/window/AbstractInputHandler.hpp>

using namespace storm;
using namespace storm::window;

/////////////////////////////////////
/////////////////////////////////////
AbstractInputHandler::AbstractInputHandler(const Window &window) : m_window { &window } {
}

/////////////////////////////////////
/////////////////////////////////////
AbstractInputHandler::~AbstractInputHandler() = default;

/////////////////////////////////////
/////////////////////////////////////
AbstractInputHandler::AbstractInputHandler(AbstractInputHandler &&) = default;

/////////////////////////////////////
/////////////////////////////////////
AbstractInputHandler &AbstractInputHandler::operator=(AbstractInputHandler &&) = default;
