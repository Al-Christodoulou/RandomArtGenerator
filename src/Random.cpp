#include "Random.h"

namespace Random {
	int get(int min, int max)
	{
		std::uniform_int_distribution range{ min,max };
		return range(mt);
	}
}