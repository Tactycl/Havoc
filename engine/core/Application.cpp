#include "Application.hpp"

namespace Havoc::Core {
	void Application::run() {
		while (!mWindowManager.isEmpty()) {
			mWindowManager.update();
		}
	}
}
