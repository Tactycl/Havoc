#include "RendererBackend.hpp"

#include "../../assetsystem/ShaderCache.hpp"

/* Refactor a bit later (Pipeline creation & ToVkFormat -> probably put into vulkan specific class) */
/* Add Vertex Buffers & upload via staging buffer */
/* Add Index Buffer & upload via staging buffer */

namespace {
	using namespace Havoc::Graphics;

	VkFormat ToVkFormat(VertexAttributeType type) {
		switch (type) {
			case VertexAttributeType::Float2: return VK_FORMAT_R32G32_SFLOAT;
			case VertexAttributeType::Float3: return VK_FORMAT_R32G32B32_SFLOAT;
			case VertexAttributeType::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
		}
		throw std::runtime_error("Unsupported type");
	}
}

namespace Havoc::Renderer::Vulkan {
	using namespace Havoc::Vulkan;

	void RendererBackend::initialize(const RendererInitializeInfo& info) {
		mCurrentFrame = 0;
		mWindow = &info.window;
		mMaxFramesInFlight = info.maxFramesInFlight;

		Vulkan::ApplicationInfo appInfo{};
		appInfo.appName = "Havoc Test";
		appInfo.appVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);

		mInstance.emplace(info.window, appInfo);
		mSurface.emplace(mInstance.value(), info.window);
		mPhysicalDevice.emplace(mInstance.value(), mSurface.value());

		mDevice.emplace(mPhysicalDevice.value());
		mSwapchain.emplace(info.window, mSurface.value(), mPhysicalDevice.value(), mDevice.value());

		mVertShader.emplace(mDevice.value(), AssetSystem::ShaderCache::loadOrCompile("shaders/src/simple.vert", Graphics::ShaderType::VERTEX), Graphics::ShaderType::VERTEX);
		mFragShader.emplace(mDevice.value(), AssetSystem::ShaderCache::loadOrCompile("shaders/src/simple.frag", Graphics::ShaderType::FRAGMENT), Graphics::ShaderType::FRAGMENT);

		Vulkan::RenderPassCreateInfo renderPassInfo{};
		renderPassInfo.format = mSwapchain.value().getVkFormat();

		mRenderPass.emplace(mDevice.value(), renderPassInfo);
		mSwapchain.value().createFramebuffers(mRenderPass.value());

		Vulkan::PipelineCreateInfo pipelineInfo{};
		pipelineInfo.shaderStages = { &mVertShader.value(), &mFragShader.value() };
		
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		pipelineInfo.colorAttachments = { colorBlendAttachment };

		auto vertexLayout = Graphics::Simple::Vertex::getLayout();

		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = vertexLayout.stride;
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		pipelineInfo.bindings = { bindingDescription };

		pipelineInfo.attributes.resize(vertexLayout.attributes.size());
		for (size_t i = 0; i < vertexLayout.attributes.size(); i++) {
			auto vertexInfo = vertexLayout.attributes[i];
			pipelineInfo.attributes[i].binding = 0;
			pipelineInfo.attributes[i].location = vertexInfo.location;
			pipelineInfo.attributes[i].format = ToVkFormat(vertexInfo.type);
			pipelineInfo.attributes[i].offset = static_cast<uint32_t>(vertexInfo.offset);
		}

		mSimplePipeline.emplace(pipelineInfo, mDevice.value(), mSwapchain.value(), mRenderPass.value());

		Vulkan::QueueFamilyIndices indices = mPhysicalDevice.value().getQueueFamilies();
		Vulkan::CommandPoolCreateInfo commandPoolInfo{};
		commandPoolInfo.queueFamilyIndex = indices.graphicsFamily.value();

		mCommandPool.emplace(mDevice.value(), commandPoolInfo);

		mCommandBuffers.reserve(mMaxFramesInFlight);
		mImageAvailableSemaphores.reserve(mMaxFramesInFlight);
		mRenderFinishedSemaphores.reserve(mMaxFramesInFlight);
		mInFlightFences.reserve(mMaxFramesInFlight);

		for (uint32_t i = 0; i < mMaxFramesInFlight; i++) {
			Vulkan::CommandBufferCreateInfo commandBufferInfo{};
			mCommandBuffers.emplace_back(mDevice.value(), mCommandPool.value(), commandBufferInfo);

			mImageAvailableSemaphores.emplace_back(mDevice.value());
			mRenderFinishedSemaphores.emplace_back(mDevice.value());

			Vulkan::FenceCreateInfo fenceInfo{};
			fenceInfo.isSignaled = true;

			mInFlightFences.emplace_back(mDevice.value(), fenceInfo);
		}

		mImagesInFlight.resize(mSwapchain.value().getImageCount(), VK_NULL_HANDLE);
	}

	void RendererBackend::shutdown() {
		vkDeviceWaitIdle(mDevice.value().getVkDevice());
	}

	void RendererBackend::recreateSwapchain() {
		int width = 0, height = 0;
		mWindow->getFramebufferSize(&width, &height);
		while (width == 0 || height == 0) {
			mWindow->getFramebufferSize(&width, &height);
			mWindow->waitEvents();
		}

		vkDeviceWaitIdle(mDevice.value().getVkDevice());

		mSwapchain.emplace(*mWindow, mSurface.value(), mPhysicalDevice.value(), mDevice.value());
		mSwapchain.value().createFramebuffers(mRenderPass.value());

		mImagesInFlight.assign(mSwapchain.value().getImageCount(), VK_NULL_HANDLE);
	}

	FrameContext RendererBackend::beginFrame() {
		uint32_t frameIndex = mCurrentFrame;

		auto& fence = mInFlightFences[frameIndex];
		fence.wait();

		uint32_t imageIndex;
		VkResult result = mSwapchain.value().acquireNextImage(&imageIndex, mImageAvailableSemaphores[frameIndex]);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapchain();
			return {};
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("[Havoc::Renderer::Vulkan::RendererBackend] Failed to acquire Swapchain Image");
		}

		fence.reset();

		if (mImagesInFlight[imageIndex] != VK_NULL_HANDLE) {
			vkWaitForFences(mDevice.value().getVkDevice(), 1, &mImagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
		}
		mImagesInFlight[imageIndex] = mInFlightFences[frameIndex].getVkFence();

		FrameContext frameContext{};
		frameContext.imageIndex = imageIndex;

		auto& cmd = mCommandBuffers[frameIndex];
		cmd.reset();
		cmd.begin();

		frameContext.commandBuffer = &cmd;
		frameContext.imageAvailableSemaphore = &mImageAvailableSemaphores[frameIndex];
		frameContext.renderFinishedSemaphore = &mRenderFinishedSemaphores[frameIndex];
		frameContext.inFlightFence = &mInFlightFences[frameIndex];

		const Framebuffer& framebuffer = mSwapchain.value().getFramebuffer(imageIndex);
		Vulkan::RenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.renderPass = &mRenderPass.value();
		renderPassBeginInfo.framebuffer = &framebuffer;
		cmd.beginRenderPass(renderPassBeginInfo);

		Vulkan::PipelineBindInfo pipelineBindInfo{};
		pipelineBindInfo.pipeline = &mSimplePipeline.value();

		cmd.bindPipeline(pipelineBindInfo);

		return frameContext;
	}

	void RendererBackend::endFrame(FrameContext& frame) {
		auto commandBuffer = static_cast<CommandBuffer*>(frame.commandBuffer);
		commandBuffer->endRenderPass();
		commandBuffer->end();

		auto waitSemaphore = static_cast<Semaphore*>(frame.imageAvailableSemaphore);
		auto signalSemaphore = static_cast<Semaphore*>(frame.renderFinishedSemaphore);

		auto inFlightFence = static_cast<Fence*>(frame.inFlightFence);

		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		VkCommandBuffer vkCommandBuffer = commandBuffer->getVkCommandBuffer();
		VkSemaphore vkWaitSemaphore = waitSemaphore->getVkSemaphore();
		VkSemaphore vkSignalSemaphore = signalSemaphore->getVkSemaphore();

		VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &vkWaitSemaphore;
		submitInfo.pWaitDstStageMask = &waitStage;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &vkCommandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &vkSignalSemaphore;

		VkResult result = vkQueueSubmit(mDevice.value().getGraphicsQueue(), 1, &submitInfo, inFlightFence->getVkFence());
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || mWindow->consumeFramebufferResized()) {
			mWindow->setFramebufferResized();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("[Havoc::Renderer::Vulkan::RendererBackend] Failed to submit Queue");
		}
	}

	void RendererBackend::draw(FrameContext& frame, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
		auto commandBuffer = static_cast<CommandBuffer*>(frame.commandBuffer);
		vkCmdDraw(commandBuffer->getVkCommandBuffer(), vertexCount, instanceCount, firstVertex, firstInstance);
	}

	void RendererBackend::present(const FrameContext& frame) {
		auto signalSemaphore = static_cast<Semaphore*>(frame.renderFinishedSemaphore);

		VkSemaphore vkSignalSemaphore = signalSemaphore->getVkSemaphore();

		VkPresentInfoKHR present{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		present.waitSemaphoreCount = 1;
		present.pWaitSemaphores = &vkSignalSemaphore;

		VkSwapchainKHR swap = mSwapchain.value().getVkSwapchainKHR();

		present.swapchainCount = 1;
		present.pSwapchains = &swap;
		present.pImageIndices = &frame.imageIndex;

		vkQueuePresentKHR(mDevice.value().getPresentQueue(), &present);

		mCurrentFrame = (mCurrentFrame + 1) % mMaxFramesInFlight;
	}
}