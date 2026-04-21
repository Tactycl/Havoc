#pragma once

#include "Instance.hpp"
#include "Surface.hpp"

#include <vulkan/vulkan.hpp>

namespace Havoc::Vulkan {
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() const {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	class PhysicalDevice {
	public:
		PhysicalDevice(const Instance& instance, const Surface& surface);
		~PhysicalDevice() = default;

		const VkPhysicalDevice& getVkPhysicalDevice() const { return mPhysicalDevice; }

		QueueFamilyIndices getQueueFamilies() const;

	private:
		const Surface& pSurface;
		VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
	};
}