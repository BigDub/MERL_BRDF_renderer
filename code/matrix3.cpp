#include "matrix3.h"

Matrix3::Matrix3()
{
	a11 = 0;
	a12 = 0;
	a13 = 0;
	a21 = 0;
	a22 = 0;
	a23 = 0;
	a31 = 0;
	a32 = 0;
	a33 = 0;
}

Matrix3::Matrix3(Vector3 v1, Vector3 v2, Vector3 v3)
{
	a11 = v1.x;
	a21 = v1.y;
	a31 = v1.z;

	a12 = v2.x;
	a22 = v2.y;
	a23 = v2.z;

	a31 = v3.x;
	a32 = v3.y;
	a33 = v3.z;
}

Matrix3::~Matrix3() {}

double Matrix3::determinate()
{
	return
		a11 * (a22*a33 - a23*a32) -
		a12 * (a21*a33 - a23*a31) +
		a13 * (a21*a32 - a22*a31);
}

Matrix3 Matrix3::transpose()
{
	Matrix3 m = Matrix3();
	m.a11 = a11;
	m.a22 = a22;
	m.a33 = a33;

	m.a12 = a21;
	m.a21 = a12;

	m.a13 = a31;
	m.a31 = a13;

	m.a23 = a32;
	m.a32 = a23;
	return m;
}

Matrix3 Matrix3::inverse()
{
	double det = determinate();
	Matrix3 m = Matrix3();
	if (det == 0) return m;
	Matrix3 t = transpose();

	m.a11 = (t.a22*t.a33 - t.a23*t.a32);
	m.a12 = -(t.a21*t.a33 - t.a23*t.a31);
	m.a13 = (t.a21*t.a32 - t.a22*t.a31);

	m.a21 = -(t.a12*t.a33 - t.a13*t.a32);
	m.a22 = (t.a11*t.a33 - t.a13*t.a31);
	m.a23 = -(t.a11*t.a32 - t.a12*t.a31);

	m.a31 = (t.a12*t.a23 - t.a13*t.a22);
	m.a32 = -(t.a11*t.a23 - t.a13*t.a21);
	m.a33 = (t.a11*t.a22 - t.a12*t.a21);

	m /= det;
	return m;
}

Matrix3 Matrix3::operator-()
{
	Matrix3 m = Matrix3();
	m.a11 = -a11;
	m.a12 = -a12;
	m.a13 = -a13;
	m.a21 = -a21;
	m.a22 = -a22;
	m.a23 = -a23;
	m.a31 = -a31;
	m.a32 = -a32;
	m.a33 = -a33;
	return m;
}

Matrix3 Matrix3::operator+(const Matrix3& o)
{
	Matrix3 m = Matrix3();
	m.a11 = a11 + o.a11;
	m.a12 = a12 + o.a12;
	m.a13 = a13 + o.a13;
	m.a21 = a21 + o.a21;
	m.a22 = a22 + o.a22;
	m.a23 = a23 + o.a23;
	m.a31 = a31 + o.a31;
	m.a32 = a32 + o.a32;
	m.a33 = a33 + o.a33;
	return m;
}

Matrix3 Matrix3::operator*(const Matrix3& o)
{
	Matrix3 m = Matrix3();
	m.a11 = a11 * o.a11 + a12 * o.a21 + a13 * o.a31;
	m.a12 = a11 * o.a12 + a12 * o.a22 + a13 * o.a32;
	m.a13 = a11 * o.a13 + a12 * o.a23 + a13 * o.a33;

	m.a21 = a21 * o.a11 + a22 * o.a21 + a23 * o.a31;
	m.a22 = a21 * o.a12 + a22 * o.a22 + a23 * o.a32;
	m.a23 = a21 * o.a13 + a22 * o.a23 + a23 * o.a33;

	m.a31 = a31 * o.a11 + a32 * o.a21 + a33 * o.a31;
	m.a32 = a31 * o.a12 + a32 * o.a22 + a33 * o.a32;
	m.a33 = a31 * o.a13 + a32 * o.a23 + a33 * o.a33;
	return m;
}

Matrix3 Matrix3::operator*(double d)
{
	Matrix3 m = Matrix3();
	m.a11 = d * a11;
	m.a12 = d * a12;
	m.a13 = d * a13;
	m.a21 = d * a21;
	m.a22 = d * a22;
	m.a23 = d * a23;
	m.a31 = d * a31;
	m.a32 = d * a32;
	m.a33 = d * a33;
	return m;
}

Matrix3 Matrix3::operator/(double d)
{
	Matrix3 m = Matrix3();
	m.a11 = a11 / d;
	m.a12 = a12 / d;
	m.a13 = a13 / d;
	m.a21 = a21 / d;
	m.a22 = a22 / d;
	m.a23 = a23 / d;
	m.a31 = a31 / d;
	m.a32 = a32 / d;
	m.a33 = a33 / d;
	return m;
}

Vector3 Matrix3::operator*(const Vector3& v)
{
	return Vector3(
		a11 * v.x + a12 * v.y + a13 * v.z,
		a21 * v.x + a22 * v.y + a23 * v.z,
		a31 * v.x + a32 * v.y + a33 * v.z
	);
}

void Matrix3::operator+=(const Matrix3& o)
{
	a11 += o.a11;
	a12 += o.a12;
	a13 += o.a13;
	a21 += o.a21;
	a22 += o.a22;
	a23 += o.a23;
	a31 += o.a31;
	a32 += o.a32;
	a33 += o.a33;
}

void Matrix3::operator-=(const Matrix3& o)
{
	a11 -= o.a11;
	a12 -= o.a12;
	a13 -= o.a13;
	a21 -= o.a21;
	a22 -= o.a22;
	a23 -= o.a23;
	a31 -= o.a31;
	a32 -= o.a32;
	a33 -= o.a33;
}

void Matrix3::operator*=(double d)
{
	a11 *= d;
	a12 *= d;
	a13 *= d;
	a21 *= d;
	a22 *= d;
	a23 *= d;
	a31 *= d;
	a32 *= d;
	a33 *= d;
}

void Matrix3::operator/=(double d)
{
	a11 /= d;
	a12 /= d;
	a13 /= d;
	a21 /= d;
	a22 /= d;
	a23 /= d;
	a31 /= d;
	a32 /= d;
	a33 /= d;
}
