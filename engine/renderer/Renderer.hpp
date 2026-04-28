#pragma once

#include "../graphics/RendererApi.hpp"
#include "FrameContext.hpp"
#include "IRendererBackend.hpp"

#include <memory>

namespace Havoc::Renderer {
	class Renderer {
	public:
		explicit Renderer(Graphics::RendererApi api);
		~Renderer() = default;

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		Renderer(Renderer&&) noexcept = default;
		Renderer& operator=(Renderer&&) noexcept = default;

		void initialize(const RendererInitializeInfo& info);
		void shutdown();

		FrameContext beginFrame();
		void endFrame(FrameContext& frame);
		void draw(FrameContext& frame, uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0);
		void present(const FrameContext& frame);

		Graphics::RendererApi getApi() const noexcept { return mApi; }
		bool isInitialized() const noexcept { return mIsInitialized; }

	private:
		Graphics::RendererApi mApi;
		std::unique_ptr<IRendererBackend> mBackend;
		bool mIsInitialized = false;

		void validateBackendExists() const;
	};
}