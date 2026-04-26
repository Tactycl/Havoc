#pragma once

#include "Device.hpp"

#include <vulkan/vulkan.h>

#include <cassert>

namespace Havoc::Vulkan {
	struct CommandPoolCreateInfo {
		uint32_t queueFamilyIndex;
		VkCommandPoolCreateFlags flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	};

	class CommandPool {
	public:
		CommandPool(const Device& device, const CommandPoolCreateInfo& info);
		~CommandPool();

		CommandPool(const CommandPool&) = delete;
		CommandPool& operator=(const CommandPool&) = delete;

		CommandPool(CommandPool&& other) noexcept : pDevice(other.pDevice), mCommandPool(other.mCommandPool) {
			other.mCommandPool = VK_NULL_HANDLE;
		}

		CommandPool& operator=(CommandPool&& other) noexcept {
			if (this != &other) {
				assert(&pDevice == &other.pDevice);
				if (mCommandPool != VK_NULL_HANDLE) {
					vkDestroyCommandPool(pDevice.getVkDevice(), mCommandPool, nullptr);
				}

				mCommandPool = other.mCommandPool;
				other.mCommandPool = VK_NULL_HANDLE;
			}
			return *this;
		}

		VkCommandPool getVkCommandPool() const noexcept { return mCommandPool; }

	private:
		const Device& pDevice;

		VkCommandPool mCommandPool = VK_NULL_HANDLE;
	};
}