#ifndef __VECTOR3_H__
#define __VECTOR3_H__

#include "math.h"
#include <stdexcept>

struct Vector3 {
	double x;
	double y;
	double z;

	Vector3();
	Vector3(double);
	Vector3(double, double, double);
	~Vector3();

	void normalize();
	double magnitude();
	Vector3 cross_product(Vector3);
	Vector3 normal();
	double dot_product(Vector3);
	double angle_between(Vector3);

	Vector3 operator-();
	Vector3 operator+(const Vector3&);
	Vector3 operator-(const Vector3&);
	Vector3 operator*(double);
	Vector3 operator/(double);
	void operator+=(const Vector3&);
	void operator-=(const Vector3&);
	void operator*=(double);
	void operator/=(double);
};

#endif
