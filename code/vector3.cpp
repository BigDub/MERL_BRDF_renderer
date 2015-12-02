#include "vector3.h"

Vector3::Vector3() {}

Vector3::Vector3(double val)
{
	x = val;
	y = val;
	z = val;
}

Vector3::Vector3(double X, double Y, double Z)
{
	x = X;
	y = Y;
	z = Z;
}

Vector3::~Vector3() {}

Vector3 Vector3::cross_product(Vector3 other)
{
	return Vector3(
		y*other.z - z*other.y,
		z*other.x - x*other.z,
		x*other.y - y*other.x
	);
}

double Vector3::dot_product(Vector3 other)
{
	return x * other.x +
		y * other.y +
		z * other.z;
}

void Vector3::normalize()
{
	double len = sqrt(x*x+y*y+z*z);
	x = x / len;
	y = y / len;
	z = z / len;
}
Vector3 Vector3::normal()
{
	double len = sqrt(x*x+y*y+z*z);
	return Vector3(
		x / len,
		y / len,
		z / len
	);
}

double Vector3::magnitude()
{
	return sqrt(x*x+y*y+z*z);
}

double Vector3::angle_between(Vector3 other)
{
	return acos( dot_product(other) / magnitude() * other.magnitude() );
}

Vector3 Vector3::operator-()
{
	return Vector3(
		-this->x,
		-this->y,
		-this->z
	);
}

Vector3 Vector3::operator+(const Vector3& other)
{
	return Vector3(
		this->x + other.x,
		this->y + other.y,
		this->z + other.z
	);
}

Vector3 Vector3::operator-(const Vector3& other)
{
	return Vector3(
		this->x - other.x,
		this->y - other.y,
		this->z - other.z
	);
}

Vector3 Vector3::operator* (double scale)
{
	return Vector3(
		this->x * scale,
		this->y * scale,
		this->z * scale
	);
}

Vector3 Vector3::operator/ (double scale)
{
	return Vector3(
		this->x / scale,
		this->y / scale,
		this->z / scale
	);
}

void Vector3::operator+=(const Vector3& other)
{
	this->x += other.x;
	this->y += other.y;
	this->z += other.z;
}

void Vector3::operator-=(const Vector3& other)
{
	this->x -= other.x;
	this->y -= other.y;
	this->z -= other.z;
}

void Vector3::operator*= (double scale)
{
	this->x *= scale;
	this->y *= scale;
	this->z *= scale;
}

void Vector3::operator/= (double scale)
{
	this->x /= scale;
	this->y /= scale;
	this->z /= scale;
}
