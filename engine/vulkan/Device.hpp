#pragma once

#include "PhysicalDevice.hpp"

#include <vulkan/vulkan.h>

namespace Havoc::Vulkan {
	class Device {
	public:
		Device(const PhysicalDevice& physicalDevice);
		~Device();

		Device(const Device&) = delete;
		Device& operator=(const Device&) = delete;

		Device(Device&& other) noexcept : mDevice(other.mDevice), mGraphicsQueue(other.mGraphicsQueue), mPresentQueue(other.mPresentQueue) {
			other.mDevice = VK_NULL_HANDLE;
			other.mGraphicsQueue = VK_NULL_HANDLE;
			other.mPresentQueue = VK_NULL_HANDLE;
		}

		Device& operator=(Device&& other) noexcept {
			if (this != &other) {
				if (mDevice != VK_NULL_HANDLE) {
					vkDestroyDevice(mDevice, nullptr);
				}

				mDevice = other.mDevice;
				mGraphicsQueue = other.mGraphicsQueue;
				mPresentQueue = other.mPresentQueue;

				other.mDevice = VK_NULL_HANDLE;
				other.mGraphicsQueue = VK_NULL_HANDLE;
				other.mPresentQueue = VK_NULL_HANDLE;
			}
			return *this;
		}

		VkDevice getVkDevice() const noexcept { return mDevice; }
		VkQueue getGraphicsQueue() const noexcept { return mGraphicsQueue; }
		VkQueue getPresentQueue() const noexcept { return mPresentQueue; }

	private:
		VkDevice mDevice = VK_NULL_HANDLE;
		VkQueue mGraphicsQueue = VK_NULL_HANDLE;
		VkQueue mPresentQueue = VK_NULL_HANDLE;
	};
}