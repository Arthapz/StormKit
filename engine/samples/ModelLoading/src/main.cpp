#include <storm/log/FileLogger.hpp>
#include <storm/log/LogHandler.hpp>

#include "App.hpp"

#include <storm/main/Main.hpp>

int main(int argc, char **argv) {
    using namespace storm;

    STORM_EXPECTS(std::filesystem::exists("./stormkit/ModelLoading"));

    // log::LogHandler::setupLogger<log::FileLogger>("log.txt");
    log::LogHandler::setupDefaultLogger();

    auto app = App {};
    app.run(argc, argv);

    return EXIT_SUCCESS;
}
