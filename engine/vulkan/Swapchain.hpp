#pragma once

#include "../platform/Window.hpp"
#include "PhysicalDevice.hpp"
#include "Surface.hpp"
#include "Device.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace Havoc::Vulkan {
	class Swapchain {
	public:
		Swapchain(const Window& window, const Surface& surface, const PhysicalDevice& physicalDevice, const Device& device);
		~Swapchain();

	private:
		const Device& pDevice;
		VkSwapchainKHR mSwapchain = VK_NULL_HANDLE;
		VkFormat mSwapChainImageFormat;
		VkExtent2D mSwapChainExtent;
		std::vector<VkImage> mSwapChainImages;
	};
}