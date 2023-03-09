#pragma once
#include <random>

namespace Random {
	static std::mt19937 mt{ std::random_device{}() };

	int get(int min, int max);
}