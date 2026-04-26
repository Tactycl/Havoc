#pragma once

#include "Device.hpp"
#include "Framebuffer.hpp"

#include <vulkan/vulkan.h>

#include <cassert>

namespace Havoc::Vulkan {
	struct RenderPassCreateInfo {
		VkFormat format;
	};

	class RenderPass {
	public:
		RenderPass(const Device& device, const RenderPassCreateInfo& info);
		~RenderPass();

		RenderPass(const RenderPass&) = delete;
		RenderPass& operator=(const RenderPass&) = delete;

		RenderPass(RenderPass&& other) noexcept : pDevice(other.pDevice), mRenderPass(other.mRenderPass) {
			other.mRenderPass = VK_NULL_HANDLE;
		}

		RenderPass& operator=(RenderPass&& other) noexcept {
			if (this != &other) {
				assert(&pDevice == &other.pDevice);

				mRenderPass = other.mRenderPass;
				other.mRenderPass = VK_NULL_HANDLE;
			}
			return *this;
		}

		VkRenderPass getVkRenderPass() const noexcept { return mRenderPass; }

	private:
		const Device& pDevice;

		VkRenderPass mRenderPass = VK_NULL_HANDLE;
	};
}