#pragma once

#include "Window.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>
#include <unordered_map>
#include <string>
#include <queue>
#include <cstdint>

namespace Havoc {
	using WindowId = uint64_t;

	struct WindowHandle {
		WindowId id = 0;

		explicit operator bool() const {
			return id != 0;
		}

		bool operator==(const WindowHandle& other) const {
			return id == other.id;
		}
	};

	class WindowManager {
	public:
		WindowManager();
		~WindowManager();

		WindowManager(const WindowManager&) = delete;
		WindowManager& operator=(const WindowManager&) = delete;

		WindowHandle createWindow(int width, int height, const std::string& windowName);

		bool isEmpty() const { return mWindows.empty(); }

		void closeWindow(WindowHandle handle);
		void update();

	private:
		inline static bool sInitialized = false;

		std::unordered_map<WindowId, std::unique_ptr<Window>> mWindows;

		WindowId mNextId = 0;
		std::queue<WindowId> mFreeIds;
	};
}
