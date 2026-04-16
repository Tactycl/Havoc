#pragma once

#include "../Window/Window.hpp"

namespace Havoc {
	class App {
	public:
		static constexpr int WIDTH = 1920;
		static constexpr int HEIGHT = 1080;

		App();
		~App();

		App(const App&) = delete;
		App& operator=(const App&) = delete;

		void run();

	private:
		void drawFrame();

		Window window{ WIDTH, HEIGHT, "Havoc Engine" };
	};
}