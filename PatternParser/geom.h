#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <math.h>

#include "pattern.h"
using namespace std;


// BASIC GEOMETRY 

// Utilities

const float EPS = 0.000001f;
const float PI_F = 3.14159265358979f;

float sum(float a, float b) {
	return a + b;
}

float min(float a, float b) {
	return a < b ? a : b;
}

float max(float a, float b) {
	return a > b ? a : b;
}



// ��������
struct Vector2 {
	float x, y;
	Vector2(Vertice v)
		:x(v.x), y(v.y) {}
	Vector2()
		:x(0.0f), y(0.0f) {}
	bool operator==(Vector2 &other) {
		return (other.x == this->x) && (other.y == this->y);
	}
	float distance(Vector2 &other) {
		return sqrtf(powf((other.x - this->x), 2) + powf((other.y - this->y), 2));
	}
};

struct Vector3 {
	float x, y, z;
	Vector3(float _x, float _y, float _z)
		:x(_x), y(_y), z(_z) {}
	Vector3(Vertice v)
		:x(v.x), y(v.y), z(v.z) {}
	bool operator ==(const Vector3 &a) const {
		return x == a.x && y == a.y && z == a.z;
	}
	bool operator !=(const Vector3 &a) const {
		return x != a.x || y != a.y || z != a.z;
	}
	// ����������
	Vector3 zero() {
		x = y = z = 0.0f;
	}
	// ���ظ������
	Vector3 operator -() const { 
		return Vector3(-x, -y, -z);
	}
	// ���ؼӼ������
	Vector3 operator +(const Vector3 &a) const {
		return Vector3(x + a.x, y + a.y, z + a.z);
	}
	Vector3 operator -(const Vector3 &a) const {
		return Vector3(x - a.x, y - a.y, z - a.z);
	}
	// ���ر����ˡ����������
	Vector3 operator *(float a) const {
		return Vector3(x*a, y*a, z*a);
	}
	Vector3 operator /(float a) const {
		float oneOverA = 1.0f / a; // û�жԳ�����
		return Vector3(x*oneOverA, y*oneOverA, z*oneOverA);
	}
	// ���أ�=�����
	Vector3 &operator +=(const Vector3 &a) {
		x += a.x; y += a.y; z += a.z;
		return *this;
	}
	Vector3 &operator -= (const Vector3 &a) {
		x -= a.x; y -= a.y; z -= a.z;
		return *this;
	}
	Vector3 &operator *=(float a) {
		x *= a; y *= a; z *= a;
		return *this;
	}
	Vector3 &operator /=(float a) {
		float oneOverA = 1.0f / a;
		x *= oneOverA; y *= oneOverA; z *= oneOverA;
		return *this;
	}
	// ������׼��
	void normalize() {
		float magSq = x * x + y * y + z * z;
		if (magSq > 0.0f) { // ������
			float oneOverMag = 1.0f / sqrt(magSq);
			x *= oneOverMag;
			y *= oneOverMag;
			z *= oneOverMag;
		}
	}
	// ������ˣ����س˷������
	float operator *(const Vector3 &a) const {
		return x * a.x + y * a.y + z * a.z;
	}
};

// ����ģ��ƽ��
float Magsq(const Vector3 &a) {
	return a.x*a.x + a.y*a.y + a.z*a.z;
}

// ������ģ
float vectorMag(const Vector3 &a) {
	return sqrt(Magsq(a));
}
// �����������Ĳ��
Vector3 crossProduct(const Vector3 &a, const Vector3 &b) {
	return Vector3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}
// �����˷�
Vector3 operator *(float k, const Vector3 &v) {
	return Vector3(k*v.x, k*v.y, k*v.z);
}
// ������������
float distance(const Vector3 &a, const Vector3 &b) {
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float dz = a.z - b.z;
	return sqrt(dx*dx + dy * dy + dz * dz);
}

float ang2D(const Vector3 &a) {
	if (Magsq(a) < EPS)return 0.0f;
	return atan2(a.y, a.x);
}

Vector3 origin(0,0,0);
bool sortByAngle(const Vector3 &a, const Vector3 &b) {
	float ang1 = ang2D(a-origin);
	float ang2 = ang2D(b-origin);
	return ang1 > ang2;
}



