#include "Fence.hpp"

#include <stdexcept>

namespace Havoc::Vulkan {
	Fence::Fence(const Device& device, const FenceCreateInfo& info) : pDevice(device) {
		VkFenceCreateInfo createInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		if (info.isSignaled) {
			createInfo.flags |= VK_FENCE_CREATE_SIGNALED_BIT;
		}

		if (vkCreateFence(device.getVkDevice(), &createInfo, nullptr, &mFence) != VK_SUCCESS) {
			throw std::runtime_error("[Havoc::Vulkan::Fence] Failed to create VkFence");
		}
	}

	Fence::~Fence() {
		if (mFence != VK_NULL_HANDLE) {
			vkDestroyFence(pDevice.getVkDevice(), mFence, nullptr);
		}
	}

	void Fence::wait(uint64_t timeout) const {
		if (mFence == VK_NULL_HANDLE) {
			return;
		}

		vkWaitForFences(pDevice.getVkDevice(), 1, &mFence, VK_TRUE, timeout);
	}

	void Fence::reset() const {
		if (mFence == VK_NULL_HANDLE) {
			return;
		}

		vkResetFences(pDevice.getVkDevice(), 1, &mFence);
	}
}