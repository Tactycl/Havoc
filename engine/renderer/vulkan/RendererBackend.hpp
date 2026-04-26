#pragma once

#include "../IRendererBackend.hpp"

#include "../../vulkan/Instance.hpp"
#include "../../vulkan/Surface.hpp"
#include "../../vulkan/PhysicalDevice.hpp"
#include "../../vulkan/Device.hpp"
#include "../../vulkan/Shader.hpp"
#include "../../vulkan/Swapchain.hpp"
#include "../../vulkan/RenderPass.hpp"
#include "../../vulkan/Pipeline.hpp"
#include "../../vulkan/CommandPool.hpp"
#include "../../vulkan/CommandBuffer.hpp"
#include "../../vulkan/Semaphore.hpp"
#include "../../vulkan/Fence.hpp"

#include <vector>
#include <optional>
#include <cstdint>

namespace Havoc::Renderer::Vulkan {
	using namespace Havoc::Vulkan;

	class RendererBackend : public IRendererBackend {
	public:
		void initialize(const RendererInitializeInfo& info) override;
		void shutdown() override;

		FrameContext beginFrame() override;
		void endFrame(FrameContext& frame) override;

		void draw(FrameContext& frame, uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0) override;

		void present(const FrameContext& frame) override;

	private:
		uint32_t mCurrentFrame = 0;
		uint32_t mMaxFramesInFlight = 1;

		Window* mWindow = nullptr;

		std::optional<Instance> mInstance;
		std::optional<Surface> mSurface;
		std::optional<PhysicalDevice> mPhysicalDevice;
		std::optional<Device> mDevice;
		std::optional<Swapchain> mSwapchain;

		std::optional<Shader> mVertShader;
		std::optional<Shader> mFragShader;

		std::optional<RenderPass> mRenderPass;
		std::optional<Pipeline> mPipeline;

		std::optional<CommandPool> mCommandPool;

		std::vector<CommandBuffer> mCommandBuffers;

		std::vector<Semaphore> mImageAvailableSemaphores;
		std::vector<Semaphore> mRenderFinishedSemaphores;
		std::vector<Fence> mInFlightFences;

		std::vector<VkFence> mImagesInFlight;

		void recreateSwapchain();
	};
}