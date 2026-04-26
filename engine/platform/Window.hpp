#pragma once

#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <string>

namespace Havoc {
	class Window {
	public:
		Window(int width, int height, std::string windowName);
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		bool shouldClose() const { return glfwWindowShouldClose(mWindow); }

		GLFWwindow* getGLFWWindow() const { return mWindow; }

		void* getNativeWindowHandle() const { return glfwGetWin32Window(mWindow); }
		void* getNativeInstanceHandle() const { return GetModuleHandle(nullptr); }

		void getFramebufferSize(int* width, int* height) const { return glfwGetFramebufferSize(mWindow, width, height); }

		auto getRequiredInstanceExtensions(uint32_t* count) const { return glfwGetRequiredInstanceExtensions(count); }

		bool isFramebufferResized() const noexcept { return mIsFramebufferResized; }
		void setFramebufferResized(bool isResized = false) noexcept { mIsFramebufferResized = isResized; }

		void waitEvents() { glfwWaitEvents(); }

		bool consumeFramebufferResized() {
			bool v = mIsFramebufferResized;
			mIsFramebufferResized = false;
			return v;
		}

	private:
		int mWidth;
		int mHeight;

		bool mIsFramebufferResized = false;

		std::string mWindowName;

		GLFWwindow* mWindow = nullptr;
	};
}
