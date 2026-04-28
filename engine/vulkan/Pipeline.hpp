#pragma once

#include "Device.hpp"
#include "Swapchain.hpp"
#include "RenderPass.hpp"
#include "Shader.hpp"

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>
#include <cassert>

namespace Havoc::Vulkan {
	struct PipelineCreateInfo {
		std::vector<Shader*> shaderStages;

		std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

		std::vector<VkVertexInputBindingDescription> bindings;
		std::vector<VkVertexInputAttributeDescription> attributes;

		VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
		VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
		VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE;

		VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;

		bool depthTestEnable = false;
		bool depthWriteEnable = false;

		std::vector<VkPipelineColorBlendAttachmentState> colorAttachments;

		std::vector<VkDescriptorSetLayout> setLayouts;
		std::vector<VkPushConstantRange> pushConstants;
	};

	class Pipeline {
	public:
		Pipeline(const PipelineCreateInfo& createInfo, const Device& device, const Swapchain& Swapchain, const RenderPass& renderPass);
		~Pipeline();

		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;

		Pipeline(Pipeline&& other) noexcept : pDevice(other.pDevice), mPipelineLayout(other.mPipelineLayout), mPipeline(other.mPipeline) {
			other.mPipelineLayout = VK_NULL_HANDLE;
			other.mPipeline = VK_NULL_HANDLE;
		}

		Pipeline& operator=(Pipeline&& other) noexcept {
			if (this != &other) {
				assert(&pDevice == &other.pDevice);
				if (mPipeline != VK_NULL_HANDLE) {
					vkDestroyPipeline(pDevice.getVkDevice(), mPipeline, nullptr);
				}
				if (mPipelineLayout != VK_NULL_HANDLE) {
					vkDestroyPipelineLayout(pDevice.getVkDevice(), mPipelineLayout, nullptr);
				}

				mPipeline = other.mPipeline;
				mPipelineLayout = other.mPipelineLayout;

				other.mPipeline = VK_NULL_HANDLE;
				other.mPipelineLayout = VK_NULL_HANDLE;
			}
			return *this;
		}

		VkPipelineLayout getVkPipelineLayout() const noexcept { return mPipelineLayout; }
		VkPipeline getVkPipeline() const noexcept { return mPipeline; }

	private:
		const Device& pDevice;

		VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
		VkPipeline mPipeline = VK_NULL_HANDLE;
	};
}