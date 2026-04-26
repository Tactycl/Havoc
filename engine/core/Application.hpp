#pragma once

#include "../platform/WindowManager.hpp"

namespace Havoc::Core {
	class Application {
	public:
		Application() : mWindowManager() {}

		WindowManager* getWindowManager() { return &mWindowManager; }

		void run();

	private:
		WindowManager mWindowManager;
	};
}
