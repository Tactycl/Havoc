#pragma once

#include "../platform/Window.hpp"
#include "Instance.hpp"

#include <vulkan/vulkan.h>

namespace Havoc::Vulkan {
	class Surface {
	public:
		Surface(const Instance& instance, const Window& window);
		~Surface();

		const VkSurfaceKHR& getVkSurfaceKHR() const { return mSurface; }

	private:
		const Instance& pInstance;
		VkSurfaceKHR mSurface = VK_NULL_HANDLE;
	};
}