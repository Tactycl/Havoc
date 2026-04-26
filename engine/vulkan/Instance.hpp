#pragma once

#include "../platform/Window.hpp"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>
#include <iostream>

namespace Havoc::Vulkan {
#ifndef NDEBUG
	const std::vector<const char*> VALIDATION_LAYERS = { "VK_LAYER_KHRONOS_validation" };
#endif

	struct ApplicationInfo {
		std::string appName;
		uint32_t appVersion;

		std::string engineName = "Havoc Engine";
		uint32_t engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);

		uint32_t apiVersion = VK_MAKE_API_VERSION(0, 1, 1, 0);
	};

	class Instance {
	public:
		Instance(const Window& window, const ApplicationInfo& applicationInfo);
		~Instance();

		Instance(const Instance&) = delete;
		Instance& operator=(const Instance&) = delete;

		Instance(Instance&&) noexcept = default;
		Instance& operator=(Instance&&) noexcept = default;

		VkInstance getVkInstance() const noexcept { return mInstance; }

	private:
		VkInstance mInstance = VK_NULL_HANDLE;
#ifndef NDEBUG
		VkDebugUtilsMessengerEXT mDebugMessenger = VK_NULL_HANDLE;

		void setupDebugMessenger();
#endif
	};
}
