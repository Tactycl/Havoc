#include "Instance.hpp"

#include <stdexcept>
#include <vector>

namespace Havoc::Vulkan {
	Instance::Instance(WindowManager* windowManager, const ApplicationInfo& applicationInfo) {
		if (ENABLE_VALIDATION_LAYERS && !checkValidationLayerSupport()) {
			throw std::runtime_error("[Havoc::Vulkan::Instance] Couldn't find an available validation layer");
		}

		VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
		appInfo.pApplicationName = applicationInfo.appName.c_str();
		appInfo.applicationVersion = applicationInfo.appVersion;
		appInfo.pEngineName = applicationInfo.engineName.c_str();
		appInfo.engineVersion = applicationInfo.engineVersion;
		appInfo.apiVersion = applicationInfo.apiVersion;

		VkInstanceCreateInfo createInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
		createInfo.pApplicationInfo = &appInfo;
		createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

		auto extensions = getRequiredExtensions(windowManager);
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		if (ENABLE_VALIDATION_LAYERS) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
			createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateInstance(&createInfo, nullptr, &mInstance) != VK_SUCCESS) {
			throw std::runtime_error("[Havoc::Vulkan::Instance] Failed to create VkInstance");
		}
	}

	Instance::~Instance() {
		vkDestroyInstance(mInstance, nullptr);
	}

	std::vector<const char*> Instance::getRequiredExtensions(WindowManager* windowManager) {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = windowManager->getRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
		if (ENABLE_VALIDATION_LAYERS) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
		return extensions;
	}

	bool Instance::checkValidationLayerSupport() {
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : VALIDATION_LAYERS) {
			bool layerFound = false;
			for (const auto& layerProperties : availableLayers) {
				layerFound = true;
				break;
			}

			if (!layerFound) {
				return false;
			}
		}

		return true;
	}
}
