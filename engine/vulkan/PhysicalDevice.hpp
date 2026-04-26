#pragma once

#include "Instance.hpp"
#include "Surface.hpp"

#include <vulkan/vulkan.h>

#include <optional>

namespace Havoc::Vulkan {
	const std::vector<const char*> DEVICE_EXTENSIONS = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() const {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct SwapchainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities{};
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;

		bool isComplete() const {
			return !formats.empty() && !presentModes.empty();
		}
	};

	class PhysicalDevice {
	public:
		PhysicalDevice(const Instance& instance, const Surface& surface);
		~PhysicalDevice() = default;

		PhysicalDevice(const PhysicalDevice&) = delete;
		PhysicalDevice& operator=(const PhysicalDevice&) = delete;

		PhysicalDevice(PhysicalDevice&&) = delete;
		PhysicalDevice& operator=(PhysicalDevice&&) = delete;

		VkPhysicalDevice getVkPhysicalDevice() const noexcept { return mPhysicalDevice; }
		QueueFamilyIndices getQueueFamilies() const noexcept;
		SwapchainSupportDetails getSwapchainSupportDetails() const noexcept;

	private:
		const Surface& pSurface;

		VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
	};
}