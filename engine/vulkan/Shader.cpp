#include "Shader.hpp"

#include <stdexcept>
#include <fstream>

namespace {
	using namespace Havoc::Vulkan;

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

	shaderc_shader_kind getShadercShaderKindFromShaderType(ShaderType shaderType) {
		switch (shaderType) {
		case ShaderType::VERTEX:
			return shaderc_vertex_shader;
		case ShaderType::FRAGMENT:
			return shaderc_fragment_shader;
		case ShaderType::COMPUTE:
			return shaderc_compute_shader;
		case ShaderType::GEOMETRY:
			return shaderc_geometry_shader;
		case ShaderType::TESSELLATION_CONTROL:
			return shaderc_tess_control_shader;
		case ShaderType::TESSELLATION_EVALUATION:
			return shaderc_tess_evaluation_shader;

		case ShaderType::RAY_GEN:
			return shaderc_raygen_shader;
		case ShaderType::ANY_HIT:
			return shaderc_anyhit_shader;
		case ShaderType::CLOSEST_HIT:
			return shaderc_closesthit_shader;
		case ShaderType::MISS:
			return shaderc_miss_shader;
		case ShaderType::INTERSECTION:
			return shaderc_intersection_shader;
		case ShaderType::CALLABLE:
			return shaderc_callable_shader;

		case ShaderType::MESH:
			return shaderc_mesh_shader;
		case ShaderType::TASK:
			return shaderc_task_shader;

		default:
			throw std::runtime_error("Unsupported ShaderType");
		}
	}
}

namespace Havoc::Vulkan {
	Shader::Shader(const Device& device, const std::string& shaderPath, ShaderType shaderType) : pDevice(device), mShaderType(shaderType) {
		std::ifstream file(shaderPath, std::ios::ate | std::ios::binary);
		if (!file.is_open()) {
			throw std::runtime_error("[Havoc::Vulkan::Shader] Failed to open file stream");
		}

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::string shaderSource(fileSize, '\0');
		file.seekg(0);
		file.read(shaderSource.data(), fileSize);

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(shaderSource, getShadercShaderKindFromShaderType(shaderType), shaderPath.c_str(), options);
		if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
			throw std::runtime_error(module.GetErrorMessage());
		}

		std::vector<uint32_t> spirv(module.cbegin(), module.cend());

		VkShaderModuleCreateInfo createInfo{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
		createInfo.codeSize = spirv.size() * sizeof(uint32_t);
		createInfo.pCode = spirv.data();
		if (vkCreateShaderModule(device.getVkDevice(), &createInfo, nullptr, &mShaderModule) != VK_SUCCESS) {
			throw std::runtime_error("[Havoc::Vulkan::Shader] Failed to create VkShaderModule");
		}
	}

	Shader::~Shader() {
		vkDestroyShaderModule(pDevice.getVkDevice(), mShaderModule, nullptr);
	}
}