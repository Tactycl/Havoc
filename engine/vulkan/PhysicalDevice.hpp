#pragma once

#include "Instance.hpp"

#include <vulkan/vulkan.hpp>

namespace Havoc::Vulkan {
	class PhysicalDevice {
	public:
		PhysicalDevice(const Instance& instance);
		~PhysicalDevice() = default;

	private:
		VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
	};
}