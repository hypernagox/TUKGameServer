#include "pch.h"
#include "func.h"

int wrapAround(const int x, const int low, const int high)
{
	assert(low < high);
	const int n = (x - low) % (high - low);
	return (n >= 0) ? (n + low) : (n + high);
}
