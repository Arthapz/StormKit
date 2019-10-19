#include <storm/log/LogHandler.hpp>

#include <storm/main/Main.hpp>

#include "App.hpp"

int main(int argc, char **argv) {
    using namespace storm;

    log::LogHandler::setupDefaultLogger();

    auto app = App {};
    app.run(argc, argv);

    return EXIT_SUCCESS;
}
