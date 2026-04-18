#include "Application.hpp"

namespace Havoc {
	void Application::run() {
		while (!mWindowManager.isEmpty()) {
			mWindowManager.update();
		}
	}
}
