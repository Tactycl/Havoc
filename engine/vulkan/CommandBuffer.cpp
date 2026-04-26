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
		mCommandBufferRecordingState = {};

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
		mCommandBufferRecordingState = {};
	}

	void CommandBuffer::beginRenderPass(const RenderPassBeginInfo& info) {
		if (!info.isComplete()) {
			throw std::runtime_error("[Havoc::Vulkan::CommandBuffer] Passed RenderPassBeginInfo isn't complete");
		}

		if (info.framebuffer->getAttachmentCount() != static_cast<uint32_t>(info.clearValues.size())) {
			throw std::runtime_error("[Havoc::Vulkan::CommandBuffer] Passed ClearValues count does not match Framebuffer attachment count");
		}

		auto framebufferExtent = info.framebuffer->getVkExtent2D();

		VkOffset2D usedOffset = info.renderArea.has_value() ? info.renderArea.value().offset : VkOffset2D{ 0, 0 };
		VkExtent2D usedExtent = info.renderArea.has_value() ? info.renderArea.value().extent : framebufferExtent;

		if (usedOffset.x < 0 || usedOffset.y < 0 || usedOffset.x + usedExtent.width > framebufferExtent.width || usedOffset.y + usedExtent.height > framebufferExtent.height) {
			throw std::runtime_error("[Havoc::Vulkan::CommandBuffer] Passed renderArea exceeds Framebuffer extent");
		}

		if (mCommandBufferState != CommandBufferState::RECORDING) {
			throw std::runtime_error("[Havoc::Vulkan::CommandBuffer] CommandBuffer is not in RECORDING state");
		}

		if (mCommandBufferRecordingState.currentRenderPass) {
			throw std::runtime_error("[Havoc::Vulkan::CommandBuffer] CommandBuffer is already inside RenderPass");
		}

		auto renderPass = info.renderPass->getVkRenderPass();
		if (renderPass != info.framebuffer->getRenderPass().getVkRenderPass()) {
			throw std::runtime_error("[Havoc::Vulkan::CommandBuffer] Passed RenderPass does not match Framebuffer RenderPass");
		}

		VkRenderPassBeginInfo renderPassInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = info.framebuffer->getVkFramebuffer();
		renderPassInfo.renderArea.offset = usedOffset;
		renderPassInfo.renderArea.extent = usedExtent;
		renderPassInfo.clearValueCount = static_cast<uint32_t>(info.clearValues.size());
		renderPassInfo.pClearValues = info.clearValues.data();

		vkCmdBeginRenderPass(mCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = (float)usedOffset.x;
		viewport.y = (float)usedOffset.y;
		viewport.width = (float)usedExtent.width;
		viewport.height = (float)usedExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = usedOffset;
		scissor.extent = usedExtent;

		vkCmdSetViewport(mCommandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(mCommandBuffer, 0, 1, &scissor);

		mCommandBufferRecordingState.currentRenderPass = renderPass;
	}

	void CommandBuffer::endRenderPass() {
		if (mCommandBufferState != CommandBufferState::RECORDING) {
			throw std::runtime_error("[Havoc::Vulkan::CommandBuffer] CommandBuffer is not in RECORDING state");
		}

		if (!mCommandBufferRecordingState.currentRenderPass) {
			throw std::runtime_error("[Havoc::Vulkan::CommandBuffer] CommandBuffer not currently inside RenderPass");
		}

		vkCmdEndRenderPass(mCommandBuffer);
		mCommandBufferRecordingState.currentRenderPass = VK_NULL_HANDLE;
	}

	void CommandBuffer::bindPipeline(const PipelineBindInfo& info) {
		if (!info.isComplete()) {
			throw std::runtime_error("[Havoc::Vulkan::CommandBuffer] Failed to bind pipeline, PipelineBindInfo is not complete");
		}

		if (mCommandBufferState != CommandBufferState::RECORDING) {
			throw std::runtime_error("[Havoc::Vulkan::CommandBuffer] CommandBuffer is not recording");
		}

		auto pipeline = info.pipeline->getVkPipeline();
		if (mCommandBufferRecordingState.boundPipeline == pipeline && mCommandBufferRecordingState.currentBindPoint == info.bindPoint) {
			return;
		}

		if (!mCommandBufferRecordingState.currentRenderPass && info.bindPoint == VK_PIPELINE_BIND_POINT_GRAPHICS) {
			throw std::runtime_error("[Havoc::Vulkan::CommandBuffer] Pipeline must be bound inside render pass for GRAPHICS bind point");
		}

		vkCmdBindPipeline(mCommandBuffer, info.bindPoint, pipeline);
		mCommandBufferRecordingState.boundPipeline = pipeline;
		mCommandBufferRecordingState.currentBindPoint = info.bindPoint;
	}
}