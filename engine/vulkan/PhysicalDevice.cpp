#include "PhysicalDevice.hpp"

#include <stdexcept>
#include <map>
#include <optional>
#include <vector>

namespace {
	Havoc::Vulkan::QueueFamilyIndices findQueueFamilies(const Havoc::Vulkan::Surface& surface, VkPhysicalDevice device) {
		Havoc::Vulkan::QueueFamilyIndices indices;

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

	int rateDeviceSuitability(const Havoc::Vulkan::Surface& surface, VkPhysicalDevice device) { // TODO: Update more to include device extension support & presentation support
		Havoc::Vulkan::QueueFamilyIndices indices = findQueueFamilies(surface, device);
		if (!indices.isComplete()) {
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
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			score += 1000;
		}
		else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
			score += 500;
		}

		score += deviceProperties.limits.maxImageDimension2D;

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

	QueueFamilyIndices PhysicalDevice::getQueueFamilies() const {
		return findQueueFamilies(pSurface, mPhysicalDevice);
	}
}