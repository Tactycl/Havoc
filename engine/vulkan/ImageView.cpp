#include "ImageView.hpp"

#include <stdexcept>

namespace Havoc::Vulkan {
	ImageView::ImageView(const Device& device, const ImageViewCreateInfo& info) : pDevice(device) {
		VkImageViewCreateInfo createInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		createInfo.image = info.image;
		createInfo.viewType = info.viewType;
		createInfo.format = info.format;

		createInfo.subresourceRange.aspectMask = info.aspectFlags;
		createInfo.subresourceRange.baseMipLevel = info.baseMipLevel;
		createInfo.subresourceRange.levelCount = info.levelCount;
		createInfo.subresourceRange.baseArrayLayer = info.baseArrayLayer;
		createInfo.subresourceRange.layerCount = info.layerCount;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		if (vkCreateImageView(pDevice.getVkDevice(), &createInfo, nullptr, &mImageView) != VK_SUCCESS) {
			throw std::runtime_error("[Havoc::Vulkan::ImageView] Failed to create VkImageView");
		}
	}

	ImageView::~ImageView() {
		vkDestroyImageView(pDevice.getVkDevice(), mImageView, nullptr);
	}
}