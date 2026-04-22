#pragma once

#include "Window.hpp"

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

		const Window& getWindow(WindowHandle handle) const;

		bool isEmpty() const { return mWindows.empty(); }

		auto getRequiredInstanceExtensions(uint32_t* count) { return glfwGetRequiredInstanceExtensions(count); }

		void closeWindow(WindowHandle handle);
		void update();

	private:
		inline static bool sInitialized = false;

		std::unordered_map<WindowId, Window> mWindows;

		WindowId mNextId = 0;
		std::queue<WindowId> mFreeIds;
	};
}
