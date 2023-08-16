#pragma once

#include <cmath>

class Vector2
{
public:
	float x, y;

	Vector2()
		: x(0.0f), y(0.0f)
	{}
	Vector2(float x, float y)
		: x(x), y(y)
	{}

	Vector2 operator+(const Vector2& other) const
	{
		return Vector2(x + other.x, y + other.y);
	}
	Vector2 operator-(const Vector2& other) const
	{
		return Vector2(x - other.x, y - other.y);
	}
	Vector2& operator+=(const Vector2& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}
	Vector2& operator-=(const Vector2& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}
	Vector2& operator/=(float divisor)
	{
		x /= divisor;
		y /= divisor;
		return *this;
	}
	Vector2 operator*(float scale) const
	{
		return Vector2(scale * x, scale * y);
	}
	Vector2 operator/(float divisor) const
	{
		return Vector2(x / divisor, y / divisor);
	}

	friend Vector2 operator*(float scale, const Vector2& v)
	{
		return Vector2(scale * v.x, scale * v.y);
	}

	bool operator==(const Vector2& right) const
	{
		return this->x == right.x && this->y == right.y;
	}

	bool operator!=(const Vector2& right) const
	{
		return this->x != right.x || this->y != right.y;
	}

	float Dot(const Vector2& other) const
	{
		return x * other.x + y * other.y;
	}

	static float Dot(const Vector2& a, const Vector2& b)
	{
		return a.x * b.x + a.y * b.y;
	}

	float Magnitude() const
	{
		return sqrtf(x * x + y * y);
	}

	float SqrMagnitude() const
	{
		return x * x + y * y;
	}

	static float Distance(const Vector2& a, const Vector2& b)
	{
		float dx = b.x - a.x;
		float dy = b.y - a.y;
		return sqrtf(dx * dx + dy * dy);
	}

	void Normalize()
	{
		*this /= Magnitude();
	}

	Vector2 Normalized()
	{
		return *this / Magnitude();
	}
};