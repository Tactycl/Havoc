#pragma once

#include "Device.hpp"

#include <vulkan/vulkan.h>

#include <cassert>

namespace Havoc::Vulkan {
	struct ImageViewCreateInfo {
		VkImage image;
		VkFormat format;
		VkImageAspectFlags aspectFlags;

		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;

		uint32_t baseMipLevel = 0;
		uint32_t levelCount = 1;

		uint32_t baseArrayLayer = 0;
		uint32_t layerCount = 1;
	};

	class ImageView {
	public:
		ImageView(const Device& device, const ImageViewCreateInfo& info);
		~ImageView();

		ImageView(const ImageView&) = delete;
		ImageView& operator=(const ImageView&) = delete;

		ImageView(ImageView&& other) noexcept : pDevice(other.pDevice), mImageView(other.mImageView) {
			other.mImageView = VK_NULL_HANDLE;
		}

		ImageView& operator=(ImageView&& other) noexcept {
			if (this != &other) {
				assert(&pDevice == &other.pDevice);
				if (mImageView != VK_NULL_HANDLE) {
					vkDestroyImageView(pDevice.getVkDevice(), mImageView, nullptr);
				}

				mImageView = other.mImageView;
				other.mImageView = VK_NULL_HANDLE;
			}
			return *this;
		}

		VkImageView getVkImageView() const noexcept { return mImageView; }

	private:
		const Device& pDevice;

		VkImageView mImageView = VK_NULL_HANDLE;
	};
}