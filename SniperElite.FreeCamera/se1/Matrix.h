#pragma once
#include "Vector.h"

class Matrix {
public:
	char pad[8];
	Vector m3;
	Vector m2;
	Vector m1;

	Vector GetForward();
	Vector GetUp();
	Vector GetRight();
};