/////////// - StormKit::window - ///////////
#include <storm/window/InputHandler.hpp>

using namespace storm;
using namespace storm::window;

namespace details {
    extern std::function<AbstractInputHandler *(const Window &window)> input_handle_create_func;
    extern std::function<void(AbstractInputHandler *)> input_handle_destroy_func;
} // namespace details

/////////////////////////////////////
/////////////////////////////////////
InputHandler::InputHandler(const Window &window) {
    m_impl = details::input_handle_create_func(window);
}

/////////////////////////////////////
/////////////////////////////////////
InputHandler::~InputHandler() {
    if (m_impl) details::input_handle_destroy_func(m_impl);
}

/////////////////////////////////////
/////////////////////////////////////
InputHandler::InputHandler(InputHandler &&) = default;

/////////////////////////////////////
/////////////////////////////////////
InputHandler &InputHandler::operator=(InputHandler &&) = default;
