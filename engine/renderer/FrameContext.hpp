#pragma once

#include <cstdint>

namespace Havoc::Renderer {
	struct FrameContext {
		uint32_t imageIndex = 0;

		void* commandBuffer = nullptr;
		void* imageAvailableSemaphore = nullptr;
		void* renderFinishedSemaphore = nullptr;
		void* inFlightFence = nullptr;

		bool isComplete() const noexcept {
			return commandBuffer != nullptr;
		}
	};
}