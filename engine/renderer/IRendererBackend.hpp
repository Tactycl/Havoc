#pragma once

#include "FrameContext.hpp"

#include "../platform/Window.hpp"

namespace Havoc::Renderer {
	struct RendererInitializeInfo {
		Window& window;
		uint32_t maxFramesInFlight = 2;
	};

	class IRendererBackend {
	public:
		virtual ~IRendererBackend() = default;

		virtual void initialize(const RendererInitializeInfo& info) = 0;
		virtual void shutdown() = 0;

		virtual FrameContext beginFrame() = 0;
		virtual void endFrame(FrameContext& frame) = 0;

		virtual void draw(FrameContext& frame, uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0) = 0;

		virtual void present(const FrameContext& frame) = 0;
	};
}