#include "Device.hpp"
#include "Instance.hpp"

#include <stdexcept>

namespace Havoc::Vulkan {
	Device::Device(const PhysicalDevice& physicalDevice) {
		QueueFamilyIndices indices = physicalDevice.getQueueFamilies();

		VkDeviceQueueCreateInfo queueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO  };
		queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
		queueCreateInfo.queueCount = 1;

		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		VkPhysicalDeviceFeatures deviceFeatures{};
		VkDeviceCreateInfo createInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO  };
		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.queueCreateInfoCount = 1;
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = 0;
		
#ifndef NDEBUG
		createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
		createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
#else
		createInfo.enabledLayerCount = 0;
#endif

		if (vkCreateDevice(physicalDevice.getVkPhysicalDevice(), &createInfo, nullptr, &mDevice) != VK_SUCCESS) {
			throw std::runtime_error("[Havoc::Vulkan::Device] Failed to create VkDevice");
		}

		vkGetDeviceQueue(mDevice, indices.graphicsFamily.value(), 0, &mGraphicsQueue);
	}

	Device::~Device() {
		vkDestroyDevice(mDevice, nullptr);
	}
}