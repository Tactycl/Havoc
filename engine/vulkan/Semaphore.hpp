#pragma once

#include "Device.hpp"

#include <vulkan/vulkan.h>

namespace Havoc::Vulkan {
	struct SemaphoreCreateInfo {

	};

	class Semaphore {
	public:
		explicit Semaphore(const Device& device, const SemaphoreCreateInfo& info = {});
		~Semaphore();

		Semaphore(const Semaphore&) = delete;
		Semaphore& operator=(const Semaphore&) = delete;

		Semaphore(Semaphore&& other) noexcept : pDevice(other.pDevice), mSemaphore(other.mSemaphore) {
			other.mSemaphore = VK_NULL_HANDLE;
		}

		Semaphore& operator=(Semaphore&& other) noexcept {
			if (this != &other) {
				if (mSemaphore != VK_NULL_HANDLE) {
					vkDestroySemaphore(pDevice.getVkDevice(), mSemaphore, nullptr);
				}

				mSemaphore = other.mSemaphore;
				other.mSemaphore = VK_NULL_HANDLE;
			}
			return *this;
		}

		VkSemaphore getVkSemaphore() const noexcept { return mSemaphore; }

	private:
		const Device& pDevice;
		VkSemaphore mSemaphore = VK_NULL_HANDLE;
	};
}