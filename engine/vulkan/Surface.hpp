#pragma once

#include "../platform/Window.hpp"
#include "Instance.hpp"

#include <vulkan/vulkan.h>

#include <cassert>

namespace Havoc::Vulkan {
	class Surface {
	public:
		Surface(const Instance& instance, const Window& window);
		~Surface();

		Surface(const Surface&) = delete;
		Surface& operator=(const Surface&) = delete;

		Surface(Surface&& other) noexcept : pInstance(other.pInstance), mSurface(other.mSurface) {
			other.mSurface = VK_NULL_HANDLE;
		}

		Surface& operator=(Surface&& other) noexcept {
			if (this != &other) {
				assert(&pInstance == &other.pInstance);
				if (mSurface != VK_NULL_HANDLE) {
					vkDestroySurfaceKHR(pInstance.getVkInstance(), mSurface, nullptr);
				}

				mSurface = other.mSurface;
				other.mSurface = VK_NULL_HANDLE;
			}

			return *this;
		}

		VkSurfaceKHR getVkSurfaceKHR() const noexcept { return mSurface; }

	private:
		const Instance& pInstance;

		VkSurfaceKHR mSurface = VK_NULL_HANDLE;
	};
}