#pragma once

#include "Device.hpp"
#include "CommandPool.hpp"
#include "RenderPass.hpp"
#include "Pipeline.hpp"

#include <vulkan/vulkan.h>

#include <cassert>
#include <vector>
#include <optional>

namespace Havoc::Vulkan {
	struct CommandBufferCreateInfo {
		VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	};

	struct CommandBufferBeginInfo {
		VkCommandBufferUsageFlags usageFlags = 0;
		const VkCommandBufferInheritanceInfo* inheritanceInfo = nullptr;
	};

	struct RenderPassBeginInfo {
		const RenderPass* renderPass = nullptr;
		const Framebuffer* framebuffer = nullptr;

		std::optional<VkRect2D> renderArea;
		std::vector<VkClearValue> clearValues = { { {{0.0f, 0.0f, 0.0f, 1.0f}} }  };

		bool isComplete() const noexcept {
			return renderPass && framebuffer && !clearValues.empty();
		}
	};

	struct PipelineBindInfo {
		const Pipeline* pipeline;
		VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

		bool isComplete() const noexcept {
			return pipeline;
		}
	};

	struct CommandBufferRecordingState {
		VkRenderPass currentRenderPass = VK_NULL_HANDLE;
		VkPipelineBindPoint currentBindPoint = VK_PIPELINE_BIND_POINT_MAX_ENUM;
		VkPipeline boundPipeline = VK_NULL_HANDLE;
	};

	enum class CommandBufferState {
		INITIAL,
		RECORDING,
		EXECUTABLE
	};

	class CommandBuffer {
	public:
		CommandBuffer(const Device& device, const CommandPool& commandPool, const CommandBufferCreateInfo& info);
		~CommandBuffer();

		CommandBuffer(const CommandBuffer&) = delete;
		CommandBuffer& operator=(const CommandBuffer&) = delete;

		CommandBuffer(CommandBuffer&& other) noexcept : pDevice(other.pDevice), pCommandPool(other.pCommandPool), mCommandBuffer(other.mCommandBuffer), mCommandBufferState(other.mCommandBufferState) {
			other.mCommandBuffer = VK_NULL_HANDLE;
		}

		CommandBuffer& operator=(CommandBuffer&& other) noexcept {
			if (this != &other) {
				assert(&pDevice == &other.pDevice);
				if (mCommandBuffer != VK_NULL_HANDLE) {
					if (mCommandBufferState == CommandBufferState::RECORDING) {
						vkEndCommandBuffer(mCommandBuffer);
					}
					vkFreeCommandBuffers(pDevice.getVkDevice(), pCommandPool.getVkCommandPool(), 1, &mCommandBuffer);
				}

				mCommandBuffer = other.mCommandBuffer;
				mCommandBufferState = other.mCommandBufferState;
				other.mCommandBuffer = VK_NULL_HANDLE;
				other.mCommandBufferState = CommandBufferState::INITIAL;
			}
			return *this;
		}

		void begin(const CommandBufferBeginInfo& info = {});
		void end();
		void reset(VkCommandBufferResetFlags resetFlags = 0);

		void beginRenderPass(const RenderPassBeginInfo& info);
		void endRenderPass();

		void bindPipeline(const PipelineBindInfo& info);

		VkCommandBuffer getVkCommandBuffer() const noexcept { return mCommandBuffer; }
		CommandBufferState getCommandBufferState() const noexcept { return mCommandBufferState; }

		bool isInsideRenderPass() const noexcept { return mCommandBufferRecordingState.currentRenderPass != VK_NULL_HANDLE; }
		VkPipelineBindPoint getCurrentBindPoint() const noexcept { return mCommandBufferRecordingState.currentBindPoint; }
		VkPipeline getBoundPipeline() const noexcept { return mCommandBufferRecordingState.boundPipeline; }

	private:
		const Device& pDevice;
		const CommandPool& pCommandPool;

		VkCommandBuffer mCommandBuffer = VK_NULL_HANDLE;
		CommandBufferState mCommandBufferState = CommandBufferState::INITIAL;
		CommandBufferRecordingState mCommandBufferRecordingState{};
	};
}