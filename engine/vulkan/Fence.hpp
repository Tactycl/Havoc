#pragma once

#include "Device.hpp"

#include <vulkan/vulkan.h>

namespace Havoc::Vulkan {
	struct FenceCreateInfo {
		bool isSignaled = false;
	};

	class Fence {
	public:
		Fence(const Device& device, const FenceCreateInfo& info = {});
		~Fence();

		Fence(Fence&& other) noexcept : pDevice(other.pDevice), mFence(other.mFence) {
			other.mFence = VK_NULL_HANDLE;
		}

		Fence& operator=(Fence&& other) noexcept {
			if (this != &other) {
				if (mFence != VK_NULL_HANDLE) {
					vkDestroyFence(pDevice.getVkDevice(), mFence, nullptr);
				}

				mFence = other.mFence;
				other.mFence = VK_NULL_HANDLE;
			}
			return *this;
		}

		void wait(uint64_t timeout = UINT64_MAX) const;
		void reset() const;

		VkFence getVkFence() const noexcept { return mFence; }

	private:
		const Device& pDevice;
		VkFence mFence = VK_NULL_HANDLE;
	};
}