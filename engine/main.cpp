#include "core/Application.hpp"

#include <iostream>

int main() {
	try {
		Havoc::Application app{};

		auto windowManager = app.getWindowManager();
		windowManager->createWindow(1920, 1080, "Havoc Engine");

		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
