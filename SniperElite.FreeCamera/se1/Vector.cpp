#include "Vector.h"
#include <cmath>

void Vector::Normalise()
{
	const float sq = MagnitudeSqr();
	if (sq > 0.0f) {
		const float invsqrt = 1.0f / sqrt(sq);
		X *= invsqrt;
		Y *= invsqrt;
		Z *= invsqrt;
	}
}