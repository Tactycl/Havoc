#include "Semaphore.hpp"

#include <stdexcept>

namespace Havoc::Vulkan {
	Semaphore::Semaphore(const Device& device, const SemaphoreCreateInfo& info) : pDevice(device) {
		VkSemaphoreCreateInfo createInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
		if (vkCreateSemaphore(device.getVkDevice(), &createInfo, nullptr, &mSemaphore) != VK_SUCCESS) {
			throw std::runtime_error("[Havoc::Vulkan::Semaphore] Failed to create VkSemaphore");
		}
	}

	Semaphore::~Semaphore() {
		if (mSemaphore != VK_NULL_HANDLE) {
			vkDestroySemaphore(pDevice.getVkDevice(), mSemaphore, nullptr);
		}
	}
}