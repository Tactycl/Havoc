#pragma once

#include "Device.hpp"

#include <vulkan/vulkan.h>

#include <vector>
#include <cassert>

namespace Havoc::Vulkan {
	class RenderPass;

	struct FramebufferCreateInfo {
		std::vector<VkImageView> attachments;

		uint32_t width;
		uint32_t height;
		
		uint32_t layers = 1;
	};

	class Framebuffer {
	public:
		Framebuffer(const Device& device, const RenderPass& renderPass, const FramebufferCreateInfo& info);
		~Framebuffer();

		Framebuffer(const Framebuffer&) = delete;
		Framebuffer& operator=(const Framebuffer&) = delete;

		Framebuffer(Framebuffer&& other) noexcept : pDevice(other.pDevice), mFramebuffer(other.mFramebuffer) {
			other.mFramebuffer = VK_NULL_HANDLE;
		}

		Framebuffer& operator=(Framebuffer&& other) noexcept {
			if (this != &other) {
				assert(&pDevice == &other.pDevice);
				if (mFramebuffer != VK_NULL_HANDLE) {
					vkDestroyFramebuffer(pDevice.getVkDevice(), mFramebuffer, nullptr);
				}

				mFramebuffer = other.mFramebuffer;
				other.mFramebuffer = VK_NULL_HANDLE;
			}
			return *this;
		}

		VkFramebuffer getVkFramebuffer() const noexcept { return mFramebuffer; }

	private:
		const Device& pDevice;

		VkFramebuffer mFramebuffer = VK_NULL_HANDLE;
	};
}