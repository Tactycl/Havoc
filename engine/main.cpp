#include "core/Application.hpp"

#include "vulkan/Instance.hpp"
#include "vulkan/Surface.hpp"
#include "vulkan/PhysicalDevice.hpp"
#include "vulkan/Device.hpp"

#include <iostream>

int main() {
	try {
		Havoc::Application app{};

		auto windowManager = app.getWindowManager();
		auto windowHandle = windowManager->createWindow(1920, 1080, "Havoc Engine");

		Havoc::Vulkan::ApplicationInfo appInfo{};
		appInfo.appName = "Havoc Test";
		appInfo.appVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);

		// Temporary (test) TODO
		Havoc::Vulkan::Instance instance{ windowManager, appInfo };
		Havoc::Vulkan::Surface surface{ instance, windowManager->getWindow(windowHandle) };
		Havoc::Vulkan::PhysicalDevice physicalDevice{ instance, surface };
		Havoc::Vulkan::Device device{ physicalDevice };
		//

		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
