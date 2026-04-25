#pragma once

#include "Device.hpp"
#include "Swapchain.hpp"
#include "RenderPass.hpp"
#include "Shader.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace Havoc::Vulkan {
	const std::vector<VkDynamicState> DYNAMIC_STATES = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	struct PipelineCreateInfo {
		std::vector<Shader*> shaderStages;
	};

	class Pipeline {
	public:
		Pipeline(const PipelineCreateInfo& createInfo, const Device& device, const Swapchain& swapchain, const RenderPass& renderPass);
		~Pipeline();

	private:
		const Device& pDevice;
		VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
		VkPipeline mPipeline = VK_NULL_HANDLE;
	};
}