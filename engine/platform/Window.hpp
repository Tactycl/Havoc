#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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

	private:
		int mWidth;
		int mHeight;

		std::string mWindowName;

		GLFWwindow* mWindow = nullptr;
	};
}
