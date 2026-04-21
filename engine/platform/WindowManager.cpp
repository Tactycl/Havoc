#include "WindowManager.hpp"

#include <stdexcept>

namespace Havoc {
	WindowManager::WindowManager() {
		if (sInitialized) {
			return;
		}

		if (!glfwInit()) {
			throw std::runtime_error("[Havoc::WindowManager] Failed to initialize GLFW");
		}

		sInitialized = true;
	}

	WindowManager::~WindowManager() {
		mWindows.clear();
		if (!sInitialized) {
			return;
		}

		glfwTerminate();
		sInitialized = false;
	}

	WindowHandle WindowManager::createWindow(int width, int height, const std::string& windowName) {
		WindowId id = 0;
		if (!mFreeIds.empty()) {
			id = mFreeIds.front();
			mFreeIds.pop();
		}
		else {
			id = ++mNextId;
		}

		mWindows.try_emplace(id, width, height, windowName);
		return WindowHandle{ id };
	}

	void WindowManager::closeWindow(WindowHandle handle) {
		auto it = mWindows.find(handle.id);
		if (it == mWindows.end()) {
			throw std::out_of_range("[Havoc::WindowManager] Invalid WindowHandle");
		}

		mWindows.erase(it);
		mFreeIds.push(handle.id);
	}

	const Window& WindowManager::getWindow(WindowHandle handle) const {
		auto it = mWindows.find(handle.id);
		if (it == mWindows.end()) {
			throw std::out_of_range("[Havoc::WindowManager] Invalid WindowHandle");
		}

		return it->second;
	}

	void WindowManager::update() {
		glfwPollEvents();

		for (auto it = mWindows.begin(); it != mWindows.end();) {
			if (it->second.shouldClose()) {
				WindowId erasedId = it->first;
				it = mWindows.erase(it);
				mFreeIds.push(erasedId);
			}
			else {
				++it;
			}
		}
	}
}
