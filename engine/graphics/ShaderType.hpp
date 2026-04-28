#pragma once

namespace Havoc::Graphics {
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
}