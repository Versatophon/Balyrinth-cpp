#pragma once

#include "SimpleMathsApi.h"

#include "Quaternionf.h"

struct Vector3f;

struct SIMPLE_MATHS_API Matrix4f
{
public:
	Matrix4f(float p00, float p01, float p02, float p03,
			 float p10, float p11, float p12, float p13,
			 float p20, float p21, float p22, float p23,
			 float p30, float p31, float p32, float p33);
	Matrix4f(const Quaternionf& pX = Quaternionf::XUnit,
			 const Quaternionf& pY = Quaternionf::YUnit,
			 const Quaternionf& pZ = Quaternionf::ZUnit,
			 const Quaternionf& pW = Quaternionf::WUnit);
	Matrix4f(const Matrix4f& pOther);
	~Matrix4f();

	void SetOrientation(Quaternionf const& pOrientation);
	void SetPosition(Vector3f const& pPosition);
	void SetScale(float const pScale);
	void SetPerspectiveProjection(float pLeft, float pRight,
								  float pBottom, float pTop,
								  float pNear, float pFar);
	void SetOrthographicProjection(float pLeft, float pRight,
								   float pBottom, float pTop,
								   float pNear, float pFar);
	Quaternionf GetQuaternion() const;
	Vector3f GetEuler() const;
	Quaternionf& operator[] (unsigned int pIndex);
	const Quaternionf& operator[] (unsigned int pIndex) const;

private:
	Quaternionf M[4];

public:
    static const Matrix4f Id;
};

SIMPLE_MATHS_API Matrix4f operator*(Matrix4f const& pLeftMatrix, Matrix4f const& pRightMatrix);
