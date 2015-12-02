#ifndef __MATRIX3_H__
#define __MATRIX3_H__

#include "vector3.h"

struct Matrix3 {
	double a11;
	double a12;
	double a13;
	double a21;
	double a22;
	double a23;
	double a31;
	double a32;
	double a33;

	Matrix3();
	Matrix3(Vector3, Vector3, Vector3);
	~Matrix3();

	double determinate();
	Matrix3 transpose();
	Matrix3 inverse();

	Matrix3 operator-();
	Matrix3 operator+(const Matrix3&);
	Matrix3 operator*(const Matrix3&);
	Matrix3 operator*(double);
	Matrix3 operator/(double);
	Vector3 operator*(const Vector3&);

	void operator+=(const Matrix3&);
	void operator-=(const Matrix3&);
	void operator*=(double);
	void operator/=(double);
};

#endif
