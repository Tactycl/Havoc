#include "ShaderCache.hpp"

#include <shaderc/shaderc.hpp>

#include <fstream>
#include <functional>

namespace {
	using namespace Havoc::Core::Graphics;

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

	std::string hashShader(const std::string& source, int shaderType) {
		std::hash<std::string> hasher;

		size_t h1 = hasher(source);
		size_t h2 = hasher(std::to_string(shaderType));

		size_t combined = h1 ^ (h2 << 1);

		return std::to_string(combined);
	}

	std::string getCachePath(const std::string& hash) {
		std::filesystem::path cacheDir = "cache/shaders";
		std::filesystem::create_directories(cacheDir);

		return (cacheDir / (hash + ".spv")).string();
	}

	std::vector<uint32_t> readSpv(const std::string& path) {
		std::ifstream file(path, std::ios::ate | std::ios::binary);
		if (!file.is_open()) {
			throw std::runtime_error("[Havoc::Renderer::ShaderCache] Failed to open file stream");
		}

		size_t fileSize = static_cast<size_t>(file.tellg());
		if (fileSize % sizeof(uint32_t) != 0) {
			throw std::runtime_error("[Havoc::Renderer::ShaderCache] Corrupted SPIR-V cache (invalid size)");
		}

		std::vector<uint32_t> spirv(fileSize / sizeof(uint32_t));

		file.seekg(0);
		file.read(reinterpret_cast<char*>(spirv.data()), fileSize);

		return spirv;
	}

	void writeSpv(const std::string& path, const std::vector<uint32_t>& spv) {
		std::filesystem::create_directories(
			std::filesystem::path(path).parent_path()
		);

		std::ofstream file(path, std::ios::binary);
		if (!file.is_open()) {
			throw std::runtime_error("[Havoc::Renderer::ShaderCache] Failed to write SPIR-V cache file");
		}

		file.write(
			reinterpret_cast<const char*>(spv.data()),
			spv.size() * sizeof(uint32_t)
		);
	}

	std::string readFileAsString(const std::string& path) {
		std::ifstream file(path, std::ios::ate | std::ios::binary);
		if (!file.is_open()) {
			throw std::runtime_error("[Havoc::Renderer::ShaderCache] Failed to open shader file");
		}

		size_t size = static_cast<size_t>(file.tellg());
		std::string buffer(size, '\0');

		file.seekg(0);
		file.read(buffer.data(), size);

		return buffer;
	}

	std::vector<uint32_t> compileShaderFromSource(const std::string& source, ShaderType shaderType) {
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, getShadercShaderKindFromShaderType(shaderType), "shader", options);
		if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
			throw std::runtime_error(module.GetErrorMessage());
		}

		return { module.cbegin(), module.cend() };
	}
}

namespace Havoc::AssetSystem {
	std::vector<uint32_t> ShaderCache::loadOrCompile(const std::string& path, Core::Graphics::ShaderType shaderType) {
		std::string source = readFileAsString(path);
		std::string hash = hashShader(source, static_cast<int>(shaderType));
		std::string cachePath = getCachePath(hash);
		if (std::filesystem::exists(cachePath)) {
			return readSpv(cachePath);
		}

		auto spirv = compileShaderFromSource(source, shaderType);
		writeSpv(cachePath, spirv);
		return spirv;
	}
}