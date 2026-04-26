#include "CommandBuffer.hpp"

#include <stdexcept>

namespace Havoc::Vulkan {
	CommandBuffer::CommandBuffer(const Device& device, const CommandPool& commandPool, const CommandBufferCreateInfo& info) : pDevice(device), pCommandPool(commandPool) {
		VkCommandBufferAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocInfo.commandPool = commandPool.getVkCommandPool();
		allocInfo.level = info.level;
		allocInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(device.getVkDevice(), &allocInfo, &mCommandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("[Havoc::Vulkan::CommandBuffer] Failed to create VkCommandBuffer");
		}
	}

	CommandBuffer::~CommandBuffer() {
		if (mCommandBuffer == VK_NULL_HANDLE) return;

		if (mCommandBufferState == CommandBufferState::RECORDING) {
			vkEndCommandBuffer(mCommandBuffer);
		}
		vkFreeCommandBuffers(pDevice.getVkDevice(), pCommandPool.getVkCommandPool(), 1, &mCommandBuffer);
	}

	void CommandBuffer::begin(const CommandBufferBeginInfo& info) {
		if (mCommandBufferState != CommandBufferState::INITIAL) {
			throw std::runtime_error("[Havoc::Vulkan::CommandBuffer] Tried to begin CommandBuffer recording, but not in INITIAL state");
		}

		VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = info.usageFlags;
		beginInfo.pInheritanceInfo = info.inheritanceInfo;

		if (vkBeginCommandBuffer(mCommandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("[Havoc::Vulkan::CommandBuffer] Failed to begin recording VkCommandBuffer");
		}

		mCommandBufferState = CommandBufferState::RECORDING;
	}

	void CommandBuffer::end() {
		if (mCommandBufferState != CommandBufferState::RECORDING) {
			throw std::runtime_error("[Havoc::Vulkan::CommandBuffer] Tried to end CommandBuffer recording, but not in RECORDING state");
		}

		if (vkEndCommandBuffer(mCommandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("[Havoc::Vulkan::CommandBuffer] Failed to end VkCommandBuffer recording");
		}

		mCommandBufferState = CommandBufferState::EXECUTABLE;
	}

	void CommandBuffer::reset(VkCommandBufferResetFlags resetFlags) {
		if (mCommandBufferState == CommandBufferState::RECORDING) {
			throw std::runtime_error("[Havoc::Vulkan::CommandBuffer] Tried to reset CommandBuffer while in RECORDING state");
		}

		vkResetCommandBuffer(mCommandBuffer, resetFlags);
		mCommandBufferState = CommandBufferState::INITIAL;
	}
}