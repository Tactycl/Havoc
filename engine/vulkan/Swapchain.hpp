#pragma once

#include "../platform/Window.hpp"
#include "PhysicalDevice.hpp"
#include "Surface.hpp"
#include "Device.hpp"
#include "ImageView.hpp"
#include "Framebuffer.hpp"

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>
#include <cassert>

namespace Havoc::Vulkan {
	class RenderPass;

	class Swapchain {
	public:
		Swapchain(const Window& window, const Surface& surface, const PhysicalDevice& physicalDevice, const Device& device);
		~Swapchain();

		Swapchain(const Swapchain&) = delete;
		Swapchain& operator=(const Swapchain&) = delete;

		Swapchain(Swapchain&& other) noexcept
			: pDevice(other.pDevice),
			mSwapchain(other.mSwapchain),
			mSwapchainImageFormat(other.mSwapchainImageFormat),
			mSwapchainExtent(other.mSwapchainExtent),
			mSwapchainImages(std::move(other.mSwapchainImages)),
			mSwapchainImageViews(std::move(other.mSwapchainImageViews)),
			mSwapchainFramebuffers(std::move(other.mSwapchainFramebuffers))
		{
			other.mSwapchain = VK_NULL_HANDLE;
		}

		Swapchain& operator=(Swapchain&& other) noexcept {
			if (this != &other) {
				assert(&pDevice == &other.pDevice);
				cleanupInternal();

				mSwapchain = other.mSwapchain;
				mSwapchainImageFormat = other.mSwapchainImageFormat;
				mSwapchainExtent = other.mSwapchainExtent;

				mSwapchainImages = std::move(other.mSwapchainImages);
				mSwapchainImageViews = std::move(other.mSwapchainImageViews);
				mSwapchainFramebuffers = std::move(other.mSwapchainFramebuffers);

				other.mSwapchain = VK_NULL_HANDLE;
			}

			return *this;
		}

		void createFramebuffers(const RenderPass& renderPass);

		VkImage getVkImage(size_t index = 0) const;
		const ImageView& getImageView(size_t index = 0) const;
		const Framebuffer& getFramebuffer(size_t index = 0) const;

		VkFormat getVkFormat() const noexcept { return mSwapchainImageFormat; }
		VkExtent2D getVkExtent2D() const noexcept { return mSwapchainExtent; }

	private:
		void cleanupInternal() noexcept;

		const Device& pDevice;

		VkSwapchainKHR mSwapchain = VK_NULL_HANDLE;
		VkFormat mSwapchainImageFormat;
		VkExtent2D mSwapchainExtent;

		std::vector<VkImage> mSwapchainImages;
		std::vector<ImageView> mSwapchainImageViews;
		std::vector<Framebuffer> mSwapchainFramebuffers;
	};
}