#include "core/Application.hpp"
#include "vulkan/Instance.hpp"

#include <iostream>

int main() {
	try {
		Havoc::Application app{};

		auto windowManager = app.getWindowManager();
		windowManager->createWindow(1920, 1080, "Havoc Engine");

		Havoc::Vulkan::ApplicationInfo appInfo{};
		appInfo.appName = "Havoc Test";
		appInfo.appVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);

		Havoc::Vulkan::Instance instance{ windowManager, appInfo }; // Temporary (test) TODO

		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
