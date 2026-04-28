#pragma once

#include "../graphics/ShaderType.hpp"

#include "Device.hpp"

#include <vulkan/vulkan.h>

#include <string>
#include <cassert>

namespace Havoc::Vulkan {
	class Shader {
	public:
		Shader(const Device& device, const std::vector<uint32_t>& spirv, Graphics::ShaderType shaderType);
		~Shader();

		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

		Shader(Shader&& other) noexcept : pDevice(other.pDevice), mEntryPoint(other.mEntryPoint), mShaderType(other.mShaderType), mShaderModule(other.mShaderModule) {
			other.mShaderModule = VK_NULL_HANDLE;
		}

		Shader& operator=(Shader&& other) noexcept {
			if (this != &other) {
				assert(&pDevice == &other.pDevice);
				if (mShaderModule != VK_NULL_HANDLE) {
					vkDestroyShaderModule(pDevice.getVkDevice(), mShaderModule, nullptr);
				}

				mShaderModule = other.mShaderModule;
				mShaderType = other.mShaderType;
				mEntryPoint = other.mEntryPoint;

				other.mShaderModule = VK_NULL_HANDLE;
			}

			return *this;
		}

		VkShaderStageFlagBits getVkShaderStage() const noexcept;
		Graphics::ShaderType getShaderType() const noexcept { return mShaderType; }
		VkShaderModule getVkShaderModule() const noexcept { return mShaderModule; }
		const std::string_view getEntryPoint() const noexcept { return mEntryPoint; }

	private:
		const Device& pDevice;

		Graphics::ShaderType mShaderType;
		VkShaderModule mShaderModule = VK_NULL_HANDLE;

		std::string_view mEntryPoint = "main";
	};
}