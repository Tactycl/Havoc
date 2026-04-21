#include "Surface.hpp"

#include <stdexcept>

namespace Havoc::Vulkan {
	Surface::Surface(const Instance& instance, const Window& window) : pInstance(instance) {
		if (glfwCreateWindowSurface(instance.getVkInstance(), window.getGLFWWindow(), nullptr, &mSurface) != VK_SUCCESS) {
			throw std::runtime_error("[Havoc::Vulkan::Surface] Failed to create GLFW Window Surface (VkSurface)");
		}
	}

	Surface::~Surface() {
		vkDestroySurfaceKHR(pInstance.getVkInstance(), mSurface, nullptr);
	}
}