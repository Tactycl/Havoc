#pragma once

#include "../platform/WindowManager.hpp"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>
#include <iostream>

namespace Havoc::Vulkan {
	#ifdef NDEBUG
		constexpr bool ENABLE_VALIDATION_LAYERS = false;
	#else
		constexpr bool ENABLE_VALIDATION_LAYERS = true;
	#endif

	const std::vector<const char*> VALIDATION_LAYERS = { "VK_LAYER_KHRONOS_validation" };

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	) {
		std::cerr << "Validation Layer: " << pCallbackData->pMessage << "\n";
		return VK_FALSE;
	}

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

		bool checkValidationLayerSupport();
	};
}
