#pragma once

#include "../platform/Window.hpp"
#include "PhysicalDevice.hpp"
#include "Surface.hpp"
#include "Device.hpp"
#include "ImageView.hpp"

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

namespace Havoc::Vulkan {
	class Swapchain {
	public:
		Swapchain(const Window& window, const Surface& surface, const PhysicalDevice& physicalDevice, const Device& device);
		~Swapchain();

		VkFormat getVkFormat() const { return mSwapChainImageFormat; }
		VkExtent2D getVkExtent2D() const { return mSwapChainExtent; }

	private:
		const Device& pDevice;

		VkSwapchainKHR mSwapchain = VK_NULL_HANDLE;
		VkFormat mSwapChainImageFormat;
		VkExtent2D mSwapChainExtent;

		std::vector<VkImage> mSwapChainImages;
		std::vector<std::unique_ptr<ImageView>> mSwapChainImageViews;
	};
}