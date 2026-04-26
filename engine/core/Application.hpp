#pragma once

#include "../platform/WindowManager.hpp"

namespace Havoc::Core {
	class Application {
	public:
		Application() : mWindowManager() {}

		WindowManager* getWindowManager() { return &mWindowManager; }

		void start() noexcept { mIsRunning = true; }
		void update() noexcept { mWindowManager.update(); }
		void end() noexcept { mIsRunning = false; }

		bool isRunning() const noexcept { return mIsRunning; }

	private:
		bool mIsRunning = false;
		WindowManager mWindowManager;
	};
}
