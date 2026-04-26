#include "PhysicalDevice.hpp"

#include <stdexcept>
#include <map>
#include <optional>
#include <vector>
#include <set>

namespace {
	using namespace Havoc::Vulkan;

	SwapchainSupportDetails querySwapchainSupport(const Surface& surface, VkPhysicalDevice device) {
		SwapchainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface.getVkSurfaceKHR(), &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.getVkSurfaceKHR(), &formatCount, nullptr);

		if (formatCount > 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.getVkSurfaceKHR(), &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.getVkSurfaceKHR(), &presentModeCount, nullptr);

		if (presentModeCount > 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.getVkSurfaceKHR(), &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	QueueFamilyIndices findQueueFamilies(const Surface& surface, VkPhysicalDevice device) {
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface.getVkSurfaceKHR(), &presentSupport);
			if (presentSupport) {
				indices.presentFamily = i;
			}

			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}

			if (indices.isComplete()) {
				break;
			}

			i++;
		}

		return indices;
	}

	bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(DEVICE_EXTENSIONS.begin(), DEVICE_EXTENSIONS.end());
		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	int rateDeviceSuitability(const Surface& surface, VkPhysicalDevice device) {
		QueueFamilyIndices indices = findQueueFamilies(surface, device);
		if (!indices.isComplete()) {
			return 0;
		}

		if (!checkDeviceExtensionSupport(device)) {
			return 0;
		}

		SwapchainSupportDetails swapchainSupportDetails = querySwapchainSupport(surface, device);
		if (!swapchainSupportDetails.isComplete()) {
			return 0;
		}

		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		if (!deviceFeatures.geometryShader) {
			return 0;
		}

		int score = 0;
		switch (deviceProperties.deviceType) {
			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
				score += 2000;
				break;

			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
				score += 1000;
				break;

			case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
				score += 700;
				break;

			default:
				score += 100;
		}

		if (indices.graphicsFamily == indices.presentFamily) {
			score += 300;
		}
		else {
			score += 150;
		}

		score += static_cast<int>(swapchainSupportDetails.formats.size()) * 10;
		score += static_cast<int>(swapchainSupportDetails.presentModes.size()) * 20;

		for (const auto& mode : swapchainSupportDetails.presentModes) {
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
				score += 300;
			}
			else if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
				score += 150;
			}
		}

		score += deviceProperties.limits.maxImageDimension2D / 1024;

		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(device, &memoryProperties);

		VkDeviceSize totalVram = 0;
		for (uint32_t i = 0; i < memoryProperties.memoryHeapCount; i++) {
			if (memoryProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
				totalVram += memoryProperties.memoryHeaps[i].size;
			}
		}

		score += static_cast<int>(totalVram / (1024 * 1024 * 1024)) * 100;

		return score;
	}
}

namespace Havoc::Vulkan {
	PhysicalDevice::PhysicalDevice(const Instance& instance, const Surface& surface) : pSurface(surface) {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance.getVkInstance(), &deviceCount, nullptr);
		if (deviceCount == 0) {
			throw std::runtime_error("[Havoc::Vulkan::PhysicalDevice] Failed to find GPUs with Vulkan support");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance.getVkInstance(), &deviceCount, devices.data());

		std::multimap<int, VkPhysicalDevice> candidates;
		for (const auto& device : devices) {
			int score = rateDeviceSuitability(surface, device);
			candidates.insert(std::make_pair(score, device));
		}

		if (candidates.rbegin()->first > 0) {
			mPhysicalDevice = candidates.rbegin()->second;
		}
		else {
			throw std::runtime_error("[Havoc::Vulkan::PhysicalDevice] Failed to find a suitable GPU");
		}
	}

	QueueFamilyIndices PhysicalDevice::getQueueFamilies() const noexcept {
		return findQueueFamilies(pSurface, mPhysicalDevice);
	}

	SwapchainSupportDetails PhysicalDevice::getSwapchainSupportDetails() const noexcept {
		return querySwapchainSupport(pSurface, mPhysicalDevice);
	}
}