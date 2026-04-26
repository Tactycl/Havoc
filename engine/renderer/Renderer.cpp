#include "Renderer.hpp"

#include "vulkan/RendererBackend.hpp"

#include <stdexcept>

namespace Havoc::Renderer {
	Renderer::Renderer(Graphics::RendererApi api) : mApi(api) {
		switch (api) {
			case Graphics::RendererApi::VULKAN:
				mBackend = std::make_unique<Vulkan::RendererBackend>();
				break;

			default:
				throw std::runtime_error("[Havoc::Renderer::Renderer] Failed to create RendererBackend");
		}
	}

	void Renderer::validateBackendExists() const {
		if (!mBackend) {
			throw std::runtime_error("[Havoc::Renderer::Renderer] RendererBackend is not created yet");
		}
	}

	void Renderer::initialize(const RendererInitializeInfo& info) {
		if (mIsInitialized) {
			throw std::runtime_error("[Havoc::Renderer::Renderer] Renderer is already initialized");
		}
		validateBackendExists();

		mBackend->initialize(info);
		mIsInitialized = true;
	}

	void Renderer::shutdown() {
		if (!mIsInitialized) {
			throw std::runtime_error("[Havoc::Renderer::Renderer] Renderer is not initialized yet");
		}
		validateBackendExists();

		mBackend->shutdown();
		mIsInitialized = false;
	}

	FrameContext Renderer::beginFrame() {
		validateBackendExists();
		return mBackend->beginFrame();
	}

	void Renderer::endFrame(FrameContext& frame) {
		validateBackendExists();
		mBackend->endFrame(frame);
	}

	void Renderer::draw(FrameContext& frame, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
		validateBackendExists();
		mBackend->draw(frame, vertexCount, instanceCount, firstVertex, firstInstance);
	}

	void Renderer::present(const FrameContext& frame) {
		validateBackendExists();
		mBackend->present(frame);
	}
}