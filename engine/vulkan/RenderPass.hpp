#pragma once

#include "Device.hpp"
#include "Framebuffer.hpp"
#include "CommandBuffer.hpp"

#include <vulkan/vulkan.h>

#include <cassert>

namespace Havoc::Vulkan {
	enum class RenderPassState {
		INITIAL,
		RECORDING
	};

	struct RenderPassCreateInfo {
		VkFormat format;
	};

	struct RenderPassBeginInfo {
		const CommandBuffer* commandBuffer = nullptr;
		const Framebuffer* framebuffer = nullptr;

		VkExtent2D extent;

		bool isComplete() const {
			return commandBuffer && framebuffer;
		}
	};

	class RenderPass {
	public:
		RenderPass(const Device& device, const RenderPassCreateInfo& info);
		~RenderPass();

		RenderPass(const RenderPass&) = delete;
		RenderPass& operator=(const RenderPass&) = delete;

		RenderPass(RenderPass&& other) noexcept : pDevice(other.pDevice), mRenderPass(other.mRenderPass), mRenderPassState(other.mRenderPassState), mVkCommandBuffer(other.mVkCommandBuffer) {
			other.mRenderPass = VK_NULL_HANDLE;
		}

		RenderPass& operator=(RenderPass&& other) noexcept {
			if (this != &other) {
				assert(&pDevice == &other.pDevice);

				mRenderPass = other.mRenderPass;

				mVkCommandBuffer = VK_NULL_HANDLE;
				mRenderPassState = RenderPassState::INITIAL;

				other.mRenderPass = VK_NULL_HANDLE;
				other.mRenderPassState = RenderPassState::INITIAL;
				other.mVkCommandBuffer = VK_NULL_HANDLE;
			}
			return *this;
		}

		void begin(const RenderPassBeginInfo& info);
		void end();

		VkRenderPass getVkRenderPass() const noexcept { return mRenderPass; }

	private:
		const Device& pDevice;

		RenderPassState mRenderPassState = RenderPassState::INITIAL;
		VkRenderPass mRenderPass = VK_NULL_HANDLE;

		VkCommandBuffer mVkCommandBuffer = VK_NULL_HANDLE;
	};
}