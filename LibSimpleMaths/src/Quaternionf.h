#pragma once

#include "SimpleMathsApi.h"

#include <iostream>

struct Vector3f;

struct SIMPLE_MATHS_API Quaternionf
{
	Quaternionf(float pX = 0.f, float pY = 0.f, float pZ = 0.f, float pW = 1.f);
	Quaternionf(const Vector3f& pAxis, const float pRadians);
	Quaternionf(const Quaternionf& pOther);
	Quaternionf(const Vector3f& pVector);
	Quaternionf(const Vector3f& pFrom, const Vector3f& pTo);
	~Quaternionf();

	void Normalize();

	Quaternionf Normalized() const;
	float SquaredNorm() const;
	float Norm() const;

	Quaternionf Conjugated() const;
	Quaternionf Inverted() const;
	Quaternionf Exponentiated() const;
	Quaternionf Logarithmated() const;

	Quaternionf GetLocalOrientationFrom(const Quaternionf& pFrom) const;
	void SetLocalOrientationFrom(const Quaternionf& pFrom, const Quaternionf& pQuaternion);

	static Quaternionf FromEuler(const Vector3f& pEuler);
	Vector3f ToEuler() const;

	Vector3f Vector() const;

	Quaternionf Transform(Quaternionf const& pQuaternion) const;
	Vector3f Transform(Vector3f const& pVector) const;

	float* Array();
	const float* Array() const;
	float& operator[] (unsigned int pIndex);
	const float& operator[] (unsigned int pIndex) const;
	Vector3f& AsVector3Ref();

	Quaternionf operator*=(Quaternionf const& pQuaternion);
	Quaternionf operator*=(float pFactor);

	float X;
	float Y;
	float Z;
	float W;

	static const Quaternionf Id;
	static const Quaternionf XUnit;
	static const Quaternionf YUnit;
	static const Quaternionf ZUnit;
	static const Quaternionf WUnit;
};

SIMPLE_MATHS_API std::ostream& operator<<(std::ostream& pStream, const Quaternionf& pQuaternion);

SIMPLE_MATHS_API Quaternionf Slerp(Quaternionf const& pFrom, Quaternionf const& pTo, const float pPosition);

SIMPLE_MATHS_API Quaternionf operator/(const Quaternionf& pQuaternion, const float& pScalar);
SIMPLE_MATHS_API Quaternionf operator*(const Quaternionf& pLeft, Quaternionf const& pRight);
SIMPLE_MATHS_API Quaternionf operator+(const Quaternionf& pLeft, Quaternionf const& pRight);
SIMPLE_MATHS_API Quaternionf operator-(const Quaternionf& pLeft, Quaternionf const& pRight);
SIMPLE_MATHS_API bool operator==(const Quaternionf& pLeft, const Quaternionf& pRight);
SIMPLE_MATHS_API bool operator!=(const Quaternionf& pLeft, const Quaternionf& pRight);
