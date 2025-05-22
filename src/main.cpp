#include "core/Application.h"
#include "views/ConsoleView.h"
#include <iostream>
#include <exception>

using namespace scummredux;

int main() {
    try {
        // Create and run application
        Application app;
        return app.run();

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        ConsoleView::error("Application crashed: " + std::string(e.what()));
        return -1;

    } catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        ConsoleView::error("Application crashed: Unknown error");
        return -1;
    }
}