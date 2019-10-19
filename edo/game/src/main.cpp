#include "EdoApplication.hpp"

#include <storm/core/Platform.hpp>
#include <storm/log/ConsoleLogger.hpp>
#include <storm/log/FileLogger.hpp>
#include <storm/log/LogHandler.hpp>

#ifdef STORM_OS_WINDOWS
#include <WinBase.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	auto argc  = __argc;
	auto &argv = __argv;
#else
int main(int argc, char **argv) {
#endif
	using storm::log::operator""_module;

	auto file_logger = true;
	if (argc > 1) {
        if (std::strcmp(argv[1], "--stdout") == 0)
            file_logger = false;
	}

	if (!file_logger)
		storm::log::LogHandler::setupLogger<storm::log::ConsoleLogger>();
	else
		storm::log::LogHandler::setupLogger<storm::log::FileLogger>("logs");

	auto str = std::string{"Starting with args: "};
	for (auto i = 0; i < argc; ++i) {
		str = fmt::format(str + " {}", argv[i]);
	}

	storm::log::LogHandler::ilog("EDO"_module, str);

	try {
        auto app = EdoApplication{};
		app.run(argc, argv);
	} catch (std::exception &e) {
		storm::log::LogHandler::flog("EDO"_module, "Exception catched !: {}",
									 e.what());
	}
	return EXIT_SUCCESS;
}
