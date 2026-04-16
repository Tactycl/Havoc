#include "../include/App/App.hpp"

#include <iostream>

int main() {
	Havoc::App app{};

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}
