#pragma once

#include "../platform/WindowManager.hpp"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>
#include <iostream>

namespace Havoc::Vulkan {
#ifndef NDEBUG
	const std::vector<const char*> VALIDATION_LAYERS = { "VK_LAYER_KHRONOS_validation" };

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	) {
		if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			std::cerr << "<== Validation Layer: ==> \n" << pCallbackData->pMessage << "\n\n";
		}

		return VK_FALSE;
	}
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
		Instance(WindowManager* windowManager, const ApplicationInfo& applicationInfo);
		~Instance();

	private:
		VkInstance mInstance;

		std::vector<const char*> getRequiredExtensions(WindowManager* windowManager);

#ifndef NDEBUG
		bool checkValidationLayerSupport();

		VkDebugUtilsMessengerEXT mDebugMessenger;
		void setupDebugMessenger();
#endif
	};
}
