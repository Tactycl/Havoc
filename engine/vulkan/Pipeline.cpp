#include "Pipeline.hpp"

#include <stdexcept>

namespace Havoc::Vulkan {
	Pipeline::Pipeline(const PipelineCreateInfo& createInfo, const Device& device, const Swapchain& Swapchain, const RenderPass& renderPass) : pDevice(device) {
		VkPipelineDynamicStateCreateInfo dynamicState{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
		dynamicState.dynamicStateCount = static_cast<uint32_t>(createInfo.dynamicStates.size());
		dynamicState.pDynamicStates = createInfo.dynamicStates.data();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(createInfo.bindings.size());
		vertexInputInfo.pVertexBindingDescriptions = createInfo.bindings.data();
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(createInfo.attributes.size());
		vertexInputInfo.pVertexAttributeDescriptions = createInfo.attributes.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
		inputAssembly.topology = createInfo.topology;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo viewportState{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizer{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = createInfo.polygonMode;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = createInfo.cullMode;
		rasterizer.frontFace = createInfo.frontFace;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;

		VkPipelineMultisampleStateCreateInfo multisampling{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = createInfo.samples;
		multisampling.minSampleShading = 1.0f;
		multisampling.pSampleMask = nullptr;
		multisampling.alphaToCoverageEnable = VK_FALSE;
		multisampling.alphaToOneEnable = VK_FALSE;

		VkPipelineDepthStencilStateCreateInfo depthStencil{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
		depthStencil.depthTestEnable = createInfo.depthTestEnable;
		depthStencil.depthWriteEnable = createInfo.depthWriteEnable;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;

		VkPipelineColorBlendStateCreateInfo colorBlending{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = static_cast<uint32_t>(createInfo.colorAttachments.size());
		colorBlending.pAttachments = createInfo.colorAttachments.data();
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(createInfo.setLayouts.size());
		pipelineLayoutInfo.pSetLayouts = createInfo.setLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(createInfo.pushConstants.size());
		pipelineLayoutInfo.pPushConstantRanges = createInfo.pushConstants.data();
		if (vkCreatePipelineLayout(device.getVkDevice(), &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("[Havoc::Vulkan::Pipeline] Failed to create VkPipelineLayout");
		}

		std::vector<VkPipelineShaderStageCreateInfo> vkShaderStages;

		vkShaderStages.reserve(createInfo.shaderStages.size());
		for (const auto& shader : createInfo.shaderStages) {
			VkPipelineShaderStageCreateInfo createInfo{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
			createInfo.stage = shader->getVkShaderStage();
			createInfo.module = shader->getVkShaderModule();
			createInfo.pName = shader->getEntryPoint().data();
			vkShaderStages.push_back(createInfo);
		}

		VkGraphicsPipelineCreateInfo pipelineInfo{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
		pipelineInfo.stageCount = static_cast<uint32_t>(vkShaderStages.size());
		pipelineInfo.pStages = vkShaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = mPipelineLayout;
		pipelineInfo.renderPass = renderPass.getVkRenderPass();
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		if (vkCreateGraphicsPipelines(device.getVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mPipeline) != VK_SUCCESS) {
			throw std::runtime_error("[Havoc::Vulkan::Pipeline] Failed to create VkPipeline");
		}
	}

	Pipeline::~Pipeline() {
		if (mPipelineLayout != VK_NULL_HANDLE) {
			if (mPipeline != VK_NULL_HANDLE) {
				vkDestroyPipeline(pDevice.getVkDevice(), mPipeline, nullptr);
			}

			vkDestroyPipelineLayout(pDevice.getVkDevice(), mPipelineLayout, nullptr);
		}
	}
}