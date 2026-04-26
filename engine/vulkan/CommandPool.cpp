#include "CommandPool.hpp"

#include <stdexcept>

namespace Havoc::Vulkan {
	CommandPool::CommandPool(const Device& device, const CommandPoolCreateInfo& info) : pDevice(device) {
		VkCommandPoolCreateInfo createInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
		createInfo.flags = info.flags;
		createInfo.queueFamilyIndex = info.queueFamilyIndex;

		if (vkCreateCommandPool(device.getVkDevice(), &createInfo, nullptr, &mCommandPool) != VK_SUCCESS) {
			throw std::runtime_error("[Havoc::Vulkan::CommandPool] Failed to create VkCommandPool");
		}
	}

	CommandPool::~CommandPool() {
		if (mCommandPool != VK_NULL_HANDLE) {
			vkDestroyCommandPool(pDevice.getVkDevice(), mCommandPool, nullptr);
		}
	}
}