#include "Window.hpp"

#include <stdexcept>

namespace Havoc {
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto havocWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		havocWindow->setFramebufferResized(true);
	}

	Window::Window(int width, int height, std::string windowName) : mWidth(width), mHeight(height), mWindowName(windowName) {
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		mWindow = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		if (!mWindow) {
			throw std::runtime_error("[Havoc::Window] Failed to create GLFW window");
		}

		glfwSetWindowUserPointer(mWindow, this);
		glfwSetFramebufferSizeCallback(mWindow, framebufferResizeCallback);
	}

	Window::~Window() {
		if (mWindow) {
			glfwDestroyWindow(mWindow);
		}
	}
}
