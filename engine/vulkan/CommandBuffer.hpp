#pragma once

#include "Device.hpp"
#include "CommandPool.hpp"

#include <vulkan/vulkan.h>

#include <cassert>

namespace Havoc::Vulkan {
	struct CommandBufferCreateInfo {
		VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	};

	struct CommandBufferBeginInfo {
		VkCommandBufferUsageFlags usageFlags = 0;
		const VkCommandBufferInheritanceInfo* inheritanceInfo = nullptr;
	};

	enum class CommandBufferState {
		INITIAL,
		RECORDING,
		EXECUTABLE
	};

	class CommandBuffer {
	public:
		CommandBuffer(const Device& device, const CommandPool& commandPool, const CommandBufferCreateInfo& info);
		~CommandBuffer();

		CommandBuffer(const CommandBuffer&) = delete;
		CommandBuffer& operator=(const CommandBuffer&) = delete;

		CommandBuffer(CommandBuffer&& other) noexcept : pDevice(other.pDevice), pCommandPool(other.pCommandPool), mCommandBuffer(other.mCommandBuffer), mCommandBufferState(other.mCommandBufferState) {
			other.mCommandBuffer = VK_NULL_HANDLE;
		}

		CommandBuffer& operator=(CommandBuffer&& other) noexcept {
			if (this != &other) {
				assert(&pDevice == &other.pDevice);
				if (mCommandBuffer != VK_NULL_HANDLE) {
					if (mCommandBufferState == CommandBufferState::RECORDING) {
						vkEndCommandBuffer(mCommandBuffer);
					}
					vkFreeCommandBuffers(pDevice.getVkDevice(), pCommandPool.getVkCommandPool(), 1, &mCommandBuffer);
				}

				mCommandBuffer = other.mCommandBuffer;
				mCommandBufferState = other.mCommandBufferState;
				other.mCommandBuffer = VK_NULL_HANDLE;
				other.mCommandBufferState = CommandBufferState::INITIAL;
			}
			return *this;
		}

		void begin(const CommandBufferBeginInfo& info);
		void end();
		void reset(VkCommandBufferResetFlags resetFlags = 0);

		VkCommandBuffer getVkCommandBuffer() const noexcept { return mCommandBuffer; }
		CommandBufferState getCommandBufferState() const noexcept { return mCommandBufferState; }

	private:
		const Device& pDevice;
		const CommandPool& pCommandPool;

		VkCommandBuffer mCommandBuffer = VK_NULL_HANDLE;
		CommandBufferState mCommandBufferState = CommandBufferState::INITIAL;
	};
}