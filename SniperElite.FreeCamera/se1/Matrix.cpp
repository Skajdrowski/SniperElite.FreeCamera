#include "Matrix.h"

Vector Matrix::GetForward()
{
	Vector side = right;
	side.Normalise();
	Vector f = CrossProduct(side, Vector(0, -1, 0));
	return f;
}

Vector Matrix::GetUp()
{
    return Vector(0, -1, 0);
}

Vector Matrix::GetRight()
{
	Vector fwd = forward;
	fwd.Normalise();
	Vector r = CrossProduct(fwd, Vector(0, 1, 0));
	return r;
}
