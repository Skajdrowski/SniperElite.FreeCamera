#pragma once
#include "Vector.h"

class Matrix {
public:
	Vector forward;
	Vector up;
	Vector right;

	Vector GetForward();
	Vector GetUp();
	Vector GetRight();
};