#pragma once

#include "Device.hpp"

#include <vulkan/vulkan.h>

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

		VkImageView getVkImageView() const { return mImageView; }

	private:
		const Device& pDevice;
		VkImageView mImageView = VK_NULL_HANDLE;
	};
}