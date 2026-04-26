#include "core/Application.hpp"

#include "renderer/Renderer.hpp"

#include <iostream>

int main() {
	using namespace Havoc;

	try {
		Core::Application app{};

		WindowManager* windowManager = app.getWindowManager();
		WindowHandle windowHandle = windowManager->createWindow(1920, 1080, "Havoc Engine");
		Window& window = windowManager->getWindow(windowHandle);

		uint32_t maxFramesInFlight = 2;
		Renderer::Renderer renderer{ Graphics::RendererApi::VULKAN };
		renderer.initialize({ window, maxFramesInFlight });

		app.start();
		while (app.isRunning()) {
			if (windowManager->isEmpty()) {
				app.end();
				break;
			}

			app.update();

			auto frame = renderer.beginFrame();
			if (!frame.isComplete()) {
				continue;
			}

			renderer.draw(frame, 6);

			renderer.endFrame(frame);
			renderer.present(frame);
		}

		renderer.shutdown();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
