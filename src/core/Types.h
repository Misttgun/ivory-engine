#pragma once

#include <bitset>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

namespace re
{
	// GLOBALS
	constexpr uint8 FPS = 60;
	constexpr uint8 FUZZY_FPS = FPS + 2;
	constexpr float TIME_PER_FRAME_MS = 1000.0f / FPS;
	constexpr float DELTA_TIME_S = 1.0f / FPS;
	constexpr float FUZZY_TIME_PER_FRAME_MS = 1000.0f / FUZZY_FPS;

	// ECS
	using Entity = uint32;
	constexpr Entity MAX_ENTITIES = 5000;
	using ComponentType = uint8;
	constexpr ComponentType MAX_COMPONENTS = 32;
	using Signature = std::bitset<MAX_COMPONENTS>;
}
