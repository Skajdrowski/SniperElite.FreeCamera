#include "Matrix.h"

Vector Matrix::GetForward() const
{
	Vector fwd(row0.Z, row1.Z, row2.Z);
	fwd.Normalise();
	return fwd;
}

Vector Matrix::GetUp() const
{
	Vector up(-row0.Y, -row1.Y, -row2.Y);
	up.Normalise();
	return up;
}

Vector Matrix::GetRight() const
{
	Vector side(row0.X, row1.X, row2.X);
	side.Normalise();
	return side;
}
