#pragma once

#include "../renderer/common/ShaderType.hpp"

#include <string>
#include <vector>
#include <filesystem>

namespace Havoc::AssetSystem {
	class ShaderCache {
	public:
		static std::vector<uint32_t> loadOrCompile(const std::string& path, Core::Graphics::ShaderType shaderType);
	};
}