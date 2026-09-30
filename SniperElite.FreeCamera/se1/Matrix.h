#pragma once
#include "Vector.h"

class Matrix {
public:
	Vector row0;
	Vector row1;
	Vector row2;

	Vector GetForward() const;
	Vector GetUp() const;
	Vector GetRight() const;
};