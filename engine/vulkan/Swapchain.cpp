#include "Swapchain.hpp"

#include "RenderPass.hpp"

#include <cstdint>
#include <limits>
#include <algorithm>
#include <stdexcept>

namespace {
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D chooseSwapExtent(const Havoc::Window& window, const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		else {
			int width, height;
			window.getFramebufferSize(&width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}
}

namespace Havoc::Vulkan {
	Swapchain::Swapchain(const Window& window, const Surface& surface, const PhysicalDevice& physicalDevice, const Device& device) : pDevice(device) {
		SwapchainSupportDetails SwapchainSupport = physicalDevice.getSwapchainSupportDetails();

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(SwapchainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(SwapchainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(window, SwapchainSupport.capabilities);

		uint32_t imageCount = SwapchainSupport.capabilities.minImageCount + 1;
		if (SwapchainSupport.capabilities.maxImageCount > 0 && imageCount > SwapchainSupport.capabilities.maxImageCount) {
			imageCount = SwapchainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR  };
		createInfo.surface = surface.getVkSurfaceKHR();
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = physicalDevice.getQueueFamilies();
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		createInfo.preTransform = SwapchainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(device.getVkDevice(), &createInfo, nullptr, &mSwapchain) != VK_SUCCESS) {
			throw std::runtime_error("[Havoc::Vulkan::Swapchain] Failed to create VkSwapchainKHR");
		}

		vkGetSwapchainImagesKHR(device.getVkDevice(), mSwapchain, &imageCount, nullptr);
		mSwapchainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device.getVkDevice(), mSwapchain, &imageCount, mSwapchainImages.data());

		mSwapchainImageFormat = surfaceFormat.format;
		mSwapchainExtent = extent;

		mSwapchainImageViews.reserve(mSwapchainImages.size());
		for (auto image : mSwapchainImages) {
			ImageViewCreateInfo viewInfo{};
			viewInfo.image = image;
			viewInfo.format = mSwapchainImageFormat;
			viewInfo.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
			mSwapchainImageViews.emplace_back(device, viewInfo);
		}
	}

	Swapchain::~Swapchain() {
		cleanupInternal();
	}

	void Swapchain::cleanupInternal() noexcept {
		mSwapchainFramebuffers.clear();
		mSwapchainImageViews.clear();
		mSwapchainImages.clear();

		if (mSwapchain != VK_NULL_HANDLE) {
			vkDestroySwapchainKHR(pDevice.getVkDevice(), mSwapchain, nullptr);
			mSwapchain = VK_NULL_HANDLE;
		}
	}

	void Swapchain::createFramebuffers(const RenderPass& renderPass) {
		mSwapchainFramebuffers.reserve(mSwapchainImageViews.size());
		for (size_t i = 0; i < mSwapchainImageViews.size(); i++) {
			FramebufferCreateInfo createInfo{};
			createInfo.attachments = { mSwapchainImageViews[i].getVkImageView()};
			createInfo.width = mSwapchainExtent.width;
			createInfo.height = mSwapchainExtent.height;
			mSwapchainFramebuffers.emplace_back(pDevice, renderPass, createInfo);
		}
	}

	VkImage Swapchain::getVkImage(size_t index) const {
		if (index < 0 || index >= mSwapchainImages.size()) {
			throw std::runtime_error("[Havoc::Vulkan::Swapchain] index for mSwapchainImages is out of bounds");
		}
		return mSwapchainImages.at(index);
	};

	const ImageView& Swapchain::getImageView(size_t index) const {
		if (index < 0 || index >= mSwapchainImages.size()) {
			throw std::runtime_error("[Havoc::Vulkan::Swapchain] index for mSwapchainImageViews is out of bounds");
		}
		return mSwapchainImageViews.at(index);
	};

	const Framebuffer& Swapchain::getFramebuffer(size_t index) const {
		if (index < 0 || index >= mSwapchainFramebuffers.size()) {
			throw std::runtime_error("[Havoc::Vulkan::Swapchain] index for mSwapchainFramebuffers is out of bounds");
		}
		return mSwapchainFramebuffers.at(index);
	};
}