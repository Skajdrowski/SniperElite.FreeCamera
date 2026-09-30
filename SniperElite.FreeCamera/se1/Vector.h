#pragma once

class Vector
{
public:
	float X, Y, Z;
	Vector() : X(0.0f), Y(0.0f), Z(0.0f) {}
	Vector(float a, float b, float c) : X(a), Y(b), Z(c) {}
	float MagnitudeSqr(void) const { return X * X + Y * Y + Z * Z; }
	void Normalise();

	const Vector& operator+=(Vector const& right) {
		X += right.X;
		Y += right.Y;
		Z += right.Z;
		return *this;
	}

	const Vector& operator-=(Vector const& right) {
		X -= right.X;
		Y -= right.Y;
		Z -= right.Z;
		return *this;
	}
};

inline Vector operator*(const Vector& left, float right)
{
	return Vector(left.X * right, left.Y * right, left.Z * right);
}
