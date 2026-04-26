#include "RenderPass.hpp"

#include <stdexcept>

namespace Havoc::Vulkan {
	RenderPass::RenderPass(const Device& device, const RenderPassCreateInfo& info) : pDevice(device) {
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = info.format;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkRenderPassCreateInfo renderPassInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		
		if (vkCreateRenderPass(device.getVkDevice(), &renderPassInfo, nullptr, &mRenderPass) != VK_SUCCESS) {
			throw std::runtime_error("[Havoc::Vulkan::RenderPass] Failed to create VkRenderPass");
		}
	}

	RenderPass::~RenderPass() {
		if (mRenderPass != VK_NULL_HANDLE) {
			vkDestroyRenderPass(pDevice.getVkDevice(), mRenderPass, nullptr);
		}
	}

	void RenderPass::begin(const RenderPassBeginInfo& info) {
		if (!info.isComplete()) {
			throw std::runtime_error("[Havoc::Vulkan::RenderPass] Passed RenderPassBeginInfo isn't complete");
		}

		if (info.commandBuffer->getCommandBufferState() != CommandBufferState::RECORDING) {
			throw std::runtime_error("[Havoc::Vulkan::RenderPass] Passed CommandBuffer is not in RECORDING state");
		}

		if (mRenderPassState != RenderPassState::INITIAL) {
			throw std::runtime_error("[Havoc::Vulkan::RenderPass] Tried to begin Render Pass, but is not in INITIAL state");
		}

		VkRenderPassBeginInfo renderPassInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		renderPassInfo.renderPass = mRenderPass;
		renderPassInfo.framebuffer = info.framebuffer->getVkFramebuffer();
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = info.extent;

		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;
		vkCmdBeginRenderPass(info.commandBuffer->getVkCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		mVkCommandBuffer = info.commandBuffer->getVkCommandBuffer();
		mRenderPassState = RenderPassState::RECORDING;
	}

	void RenderPass::end() {
		if (mRenderPassState != RenderPassState::RECORDING) {
			throw std::runtime_error("[Havoc::Vulkan::RenderPass] Tried to end Render Pass, but is not in RECORDING state");
		}

		if (mVkCommandBuffer == VK_NULL_HANDLE) {
			throw std::runtime_error("[Havoc::Vulkan::RenderPass] RenderPass end called with invalid command buffer");
		}

		vkCmdEndRenderPass(mVkCommandBuffer);

		mVkCommandBuffer = VK_NULL_HANDLE;
		mRenderPassState = RenderPassState::INITIAL;
	}
}