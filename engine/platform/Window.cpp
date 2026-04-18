#include "Window.hpp"

#include <stdexcept>

namespace Havoc {
	Window::Window(int width, int height, std::string windowName) : mWidth(width), mHeight(height), mWindowName(windowName) {
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		mWindow = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		if (!mWindow) {
			throw std::runtime_error("[Havoc::Window] Failed to create GLFW window");
		}
	}

	Window::~Window() {
		if (mWindow) {
			glfwDestroyWindow(mWindow);
		}
	}
}
