#pragma once
#include <cstddef>
#include "Matrix.h"

struct Camera {
	Vector Position;
	Matrix Rotation;
};

Camera* GetCamera();