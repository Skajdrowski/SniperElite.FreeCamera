#include "Matrix.h"

Vector Matrix::GetForward()
{
	return m3;
}

Vector Matrix::GetUp()
{
	return m2;
}

Vector Matrix::GetRight()
{
	return m1;
}
