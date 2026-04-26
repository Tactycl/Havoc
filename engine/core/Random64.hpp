#pragma once

#include <random>
#include <cstdint>
#include <limits>

namespace Havoc::Core {
	class Random64 {
	public:
		Random64() : mRandomDevice(), mRng(mRandomDevice()), mDistribution(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max()) {}

		explicit Random64(uint64_t seed) : mRandomDevice(), mRng(seed), mDistribution(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max()) {}

		uint64_t next() { return mDistribution(mRng); }

	private:
		std::random_device mRandomDevice;
		std::mt19937_64 mRng;
		std::uniform_int_distribution<uint64_t> mDistribution;
	};
}