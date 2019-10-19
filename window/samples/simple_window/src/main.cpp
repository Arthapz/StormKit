#include <storm/log/LogHandler.hpp>
#include <storm/window/Window.hpp>

int main() {
	using namespace storm::window;
	using namespace storm::log;

	LogHandler::setupDefaultLogger();

	auto window = Window{"Hello world", VideoSettings{.size = {800u, 600u}},
						 WindowStyle::All};

	while (window.isOpen()) {
		auto event = Event{};
		while (window.pollEvent(event)) {
			switch (event.type) {
			case EventType::Closed:
				window.close();
				break;
			case EventType::Resized: {
				const auto size = event.resizedEvent;
				LogHandler::ilog("Resize event: {1}:{2}", size.width,
								 size.height);
				break;
			}
			case EventType::MouseMoved: {
				const auto position = event.mouseEvent;
				LogHandler::ilog("Mouse move event: {1}:{2}", position.x,
								 position.y);
				break;
			}
			case EventType::MouseButtonPushed: {
				const auto button = event.mouseEvent;
				LogHandler::ilog("Mouse button push event: {1} {2}:{3}",
								 button.button, button.x, button.y);
				break;
			}
			case EventType::MouseButtonReleased: {
				const auto button = event.mouseEvent;
				LogHandler::ilog("Mouse button release event: {1} {2}:{3}",
								 button.button, button.x, button.y);
				break;
			}
			case EventType::MouseEntered: {
				LogHandler::ilog("Mouse Entered event");
				break;
			}
			case EventType::MouseExited: {
				LogHandler::ilog("Mouse Exited event");
				break;
			}
			case EventType::KeyPressed: {
				const auto key = event.keyEvent.key;

				if (event.keyEvent.key == Key::ESCAPE) {
					window.close();
					break;
				}

				LogHandler::ilog("Key pressed: {1}", key);
				break;
			}
			case EventType::KeyReleased: {
				const auto key = event.keyEvent.key;

				if (event.keyEvent.key == Key::ESCAPE) {
					window.close();
					break;
				}

				LogHandler::ilog("Key release: {1}", key);
				break;
			}
			}
		}

		window.display();
	}

	return EXIT_SUCCESS;
}
