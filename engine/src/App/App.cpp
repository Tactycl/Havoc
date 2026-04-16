#include "../../include/App/App.hpp"

#include <print>

namespace Havoc {
	App::App() {

	}

	App::~App() {

	}

	void App::run() {
		while (!window.shouldClose()) {
			glfwPollEvents();
			drawFrame();
		}

		//vkDeviceWaitIdle(VkDeviceHere);
	}

	void App::drawFrame() {

	}
}