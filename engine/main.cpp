#include "core/Application.hpp"

#include "vulkan/Instance.hpp"
#include "vulkan/Surface.hpp"
#include "vulkan/PhysicalDevice.hpp"
#include "vulkan/Device.hpp"
#include "vulkan/Swapchain.hpp"
#include "vulkan/Shader.hpp"
#include "vulkan/RenderPass.hpp"
#include "vulkan/Pipeline.hpp"

#include <iostream>

int main() {
	using namespace Havoc;

	try {
		Application app{};

		auto windowManager = app.getWindowManager();
		auto windowHandle = windowManager->createWindow(1920, 1080, "Havoc Engine");

		// Temporary (test) TODO
		Vulkan::ApplicationInfo appInfo{};
		appInfo.appName = "Havoc Test";
		appInfo.appVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);

		const Window& window = windowManager->getWindow(windowHandle);
		Vulkan::Instance instance{ windowManager, appInfo };
		Vulkan::Surface surface{ instance, window };
		Vulkan::PhysicalDevice physicalDevice{ instance, surface };
		Vulkan::Device device{ physicalDevice };
		Vulkan::Swapchain swapchain{ window, surface, physicalDevice, device };
		Vulkan::Shader vertShader{ device, "shaders/shader.vert", Vulkan::ShaderType::VERTEX};
		Vulkan::Shader fragShader{ device, "shaders/shader.frag", Vulkan::ShaderType::FRAGMENT};

		Vulkan::PipelineCreateInfo pipelineInfo{};
		pipelineInfo.shaderStages = { &vertShader, &fragShader };

		Vulkan::RenderPass renderPass{ device, swapchain };
		Vulkan::Pipeline pipeline{ pipelineInfo, device, swapchain, renderPass };
		//

		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
