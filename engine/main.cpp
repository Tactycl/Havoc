#include "assetsystem/ShaderCache.hpp"

#include "core/Application.hpp"

#include "core/graphics/ShaderType.hpp"

#include "vulkan/Instance.hpp"
#include "vulkan/Surface.hpp"
#include "vulkan/PhysicalDevice.hpp"
#include "vulkan/Device.hpp"
#include "vulkan/Swapchain.hpp"
#include "vulkan/Shader.hpp"
#include "vulkan/RenderPass.hpp"
#include "vulkan/Pipeline.hpp"
#include "vulkan/CommandPool.hpp"
#include "vulkan/CommandBuffer.hpp"

#include <iostream>

int main() {
	using namespace Havoc;

	try {
		Core::Application app{};

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
		Vulkan::Shader vertShader{ device, AssetSystem::ShaderCache::loadOrCompile("shaders/shader.vert", Core::Graphics::ShaderType::VERTEX), Core::Graphics::ShaderType::VERTEX};
		Vulkan::Shader fragShader{ device, AssetSystem::ShaderCache::loadOrCompile("shaders/shader.frag", Core::Graphics::ShaderType::FRAGMENT), Core::Graphics::ShaderType::FRAGMENT };

		Vulkan::RenderPassCreateInfo renderPassInfo{};
		renderPassInfo.format = swapchain.getVkFormat();

		Vulkan::RenderPass renderPass{ device, renderPassInfo };
		swapchain.createFramebuffers(renderPass);

		Vulkan::PipelineCreateInfo pipelineInfo{};
		pipelineInfo.shaderStages = { &vertShader, &fragShader };

		Vulkan::Pipeline pipeline{ pipelineInfo, device, swapchain, renderPass };

		Vulkan::QueueFamilyIndices indices = physicalDevice.getQueueFamilies();
		Vulkan::CommandPoolCreateInfo commandPoolInfo{};
		commandPoolInfo.queueFamilyIndex = indices.graphicsFamily.value();

		Vulkan::CommandPool commandPool{ device, commandPoolInfo };

		Vulkan::CommandBufferCreateInfo commandBufferInfo{};
		Vulkan::CommandBuffer commandBuffer{ device, commandPool, commandBufferInfo };

		Vulkan::CommandBufferBeginInfo commandBufferBeginInfo{};
		commandBuffer.begin(commandBufferBeginInfo);

		Vulkan::RenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.commandBuffer = &commandBuffer;
		renderPassBeginInfo.framebuffer = &swapchain.getFramebuffer(0);
		renderPassBeginInfo.extent = swapchain.getVkExtent2D();
		renderPass.begin(renderPassBeginInfo);

		// TODO: Make Renderer & FrameContext

		renderPass.end();
		commandBuffer.end();
		//

		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
