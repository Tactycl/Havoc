#include "PhysicalDevice.hpp"

#include <stdexcept>
#include <map>
#include <optional>
#include <vector>

namespace {
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;

		bool isComplete() const {
			return graphicsFamily.has_value();
		}
	};

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
				break;
			}

			i++;
		}

		return indices;
	}

	int rateDeviceSuitability(VkPhysicalDevice device) { // TODO: Update more to include device extension support & presentation support
		QueueFamilyIndices indices = findQueueFamilies(device);
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
	PhysicalDevice::PhysicalDevice(const Instance& instance) {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance.getVkInstance(), &deviceCount, nullptr);
		if (deviceCount == 0) {
			throw std::runtime_error("[Havoc::Vulkan::PhysicalDevice] Failed to find GPUs with Vulkan support");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance.getVkInstance(), &deviceCount, devices.data());

		std::multimap<int, VkPhysicalDevice> candidates;
		for (const auto& device : devices) {
			int score = rateDeviceSuitability(device);
			candidates.insert(std::make_pair(score, device));
		}

		if (candidates.rbegin()->first > 0) {
			mPhysicalDevice = candidates.rbegin()->second;
		}
		else {
			throw std::runtime_error("[Havoc::Vulkan::PhysicalDevice] Failed to find a suitable GPU");
		}
	}
}