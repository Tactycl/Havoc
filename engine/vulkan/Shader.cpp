#include "Shader.hpp"

#include <stdexcept>
#include <fstream>

namespace {
	using namespace Havoc::Graphics;

	VkShaderStageFlagBits getVkShaderStageFromShaderType(ShaderType shaderType) {
		switch (shaderType) {
			case ShaderType::VERTEX:
				return VK_SHADER_STAGE_VERTEX_BIT;
			case ShaderType::FRAGMENT:
				return VK_SHADER_STAGE_FRAGMENT_BIT;
			case ShaderType::COMPUTE:
				return VK_SHADER_STAGE_COMPUTE_BIT;
			case ShaderType::GEOMETRY:
				return VK_SHADER_STAGE_GEOMETRY_BIT;
			case ShaderType::TESSELLATION_CONTROL:
				return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
			case ShaderType::TESSELLATION_EVALUATION:
				return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

			case ShaderType::RAY_GEN:
				return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
			case ShaderType::ANY_HIT:
				return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
			case ShaderType::CLOSEST_HIT:
				return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
			case ShaderType::MISS:
				return VK_SHADER_STAGE_MISS_BIT_KHR;
			case ShaderType::INTERSECTION:
				return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
			case ShaderType::CALLABLE:
				return VK_SHADER_STAGE_CALLABLE_BIT_KHR;

			case ShaderType::MESH:
				return VK_SHADER_STAGE_MESH_BIT_EXT;
			case ShaderType::TASK:
				return VK_SHADER_STAGE_TASK_BIT_EXT;

			default:
				throw std::runtime_error("Unsupported ShaderType");
		}
	}
}

namespace Havoc::Vulkan {
	Shader::Shader(const Device& device, const std::vector<uint32_t>& spirv, Graphics::ShaderType shaderType) : pDevice(device), mShaderType(shaderType) {
		if (spirv.empty()) {
			throw std::runtime_error("[Havoc::Vulkan::Shader] Empty SPIR-V passed to Shader");
		}

		VkShaderModuleCreateInfo createInfo{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
		createInfo.codeSize = spirv.size() * sizeof(uint32_t);
		createInfo.pCode = spirv.data();
		if (vkCreateShaderModule(device.getVkDevice(), &createInfo, nullptr, &mShaderModule) != VK_SUCCESS) {
			throw std::runtime_error("[Havoc::Vulkan::Shader] Failed to create VkShaderModule");
		}
	}

	Shader::~Shader() {
		if (mShaderModule != VK_NULL_HANDLE) {
			vkDestroyShaderModule(pDevice.getVkDevice(), mShaderModule, nullptr);
		}
	}

	VkShaderStageFlagBits Shader::getVkShaderStage() const noexcept {
		return getVkShaderStageFromShaderType(mShaderType);
	}
}