#pragma once

#include "PhysicalDevice.hpp"

#include <vulkan/vulkan.hpp>

namespace Havoc::Vulkan {
	class Device {
	public:
		Device(const PhysicalDevice& physicalDevice);
		~Device();

		const VkDevice& getVkDevice() const { return mDevice; }
		const VkQueue& getGraphicsQueue() const { return mGraphicsQueue; }
		const VkQueue& getPresentQueue() const { return mPresentQueue; }

	private:
		VkDevice mDevice = VK_NULL_HANDLE;
		VkQueue mGraphicsQueue = VK_NULL_HANDLE;
		VkQueue mPresentQueue = VK_NULL_HANDLE;
	};
}