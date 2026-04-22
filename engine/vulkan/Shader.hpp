#pragma once

#include "Device.hpp"

#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>

#include <string>

namespace Havoc::Vulkan {
	enum class ShaderType {
		VERTEX,
		FRAGMENT,
		COMPUTE,
		GEOMETRY,
		TESSELLATION_CONTROL,
		TESSELLATION_EVALUATION,

		RAY_GEN,
		ANY_HIT,
		CLOSEST_HIT,
		MISS,
		INTERSECTION,
		CALLABLE,

		MESH,
		TASK,
	};

	class Shader {
	public:
		Shader(const Device& device, const std::string& shaderPath, ShaderType shaderType);
		~Shader();

		const VkShaderModule& getVkShaderModule() const { return mShaderModule; }

	private:
		const Device& pDevice;
		ShaderType mShaderType;
		VkShaderModule mShaderModule = VK_NULL_HANDLE;
	};
}