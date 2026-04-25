#pragma once

#include "Device.hpp"
#include "Swapchain.hpp"

#include <vulkan/vulkan.h>

namespace Havoc::Vulkan {
	class RenderPass {
	public:
		RenderPass(const Device& device, const Swapchain& swapchain);
		~RenderPass();

		VkRenderPass getVkRenderPass() const { return mRenderPass; }

	private:
		const Device& pDevice;
		VkRenderPass mRenderPass = VK_NULL_HANDLE;
	};
}