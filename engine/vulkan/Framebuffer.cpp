#include "Framebuffer.hpp"

#include "RenderPass.hpp"

#include <stdexcept>

namespace Havoc::Vulkan {
	Framebuffer::Framebuffer(const Device& device, const RenderPass& renderPass, const FramebufferCreateInfo& info) : pDevice(device) {
		if (info.attachments.empty()) {
			throw std::runtime_error("[Havoc::Vulkan::Framebuffer] Framebuffer requires at least one attachment");
		}

		VkFramebufferCreateInfo createInfo{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
		createInfo.renderPass = renderPass.getVkRenderPass();
		createInfo.attachmentCount = static_cast<uint32_t>(info.attachments.size());
		createInfo.pAttachments = info.attachments.data();
		createInfo.width = info.width;
		createInfo.height = info.height;
		createInfo.layers = info.layers;

		if (vkCreateFramebuffer(device.getVkDevice(), &createInfo, nullptr, &mFramebuffer) != VK_SUCCESS) {
			throw std::runtime_error("[Havoc::Vulkan::Framebuffer] Failed to create VkFramebuffer");
		}
	}

	Framebuffer::~Framebuffer() {
		if (mFramebuffer != VK_NULL_HANDLE) {
			vkDestroyFramebuffer(pDevice.getVkDevice(), mFramebuffer, nullptr);
		}
	}
}