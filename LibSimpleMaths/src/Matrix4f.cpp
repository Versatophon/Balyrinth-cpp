#include "Matrix4f.h"

#include "Vector3f.h"
#include "Quaternionf.h"

Matrix4f::Matrix4f(float p00, float p01, float p02, float p03,
				   float p10, float p11, float p12, float p13,
				   float p20, float p21, float p22, float p23,
				   float p30, float p31, float p32, float p33) :
	M{ { p00, p01, p02, p03 },
	   { p10, p11, p12, p13 },
	   { p20, p21, p22, p23 },
	   { p30, p31, p32, p33 } }
{
}


Matrix4f::Matrix4f(const Quaternionf& pX,
	const Quaternionf& pY,
	const Quaternionf& pZ,
	const Quaternionf& pW) :
	M{ pX, pY, pZ, pW }
{
}

Matrix4f::Matrix4f(const Matrix4f& pOther):
	M{ pOther.M[0], pOther.M[1], pOther.M[2], pOther.M[3] }
{
}

Matrix4f::~Matrix4f()
{
}

void Matrix4f::SetOrientation(Quaternionf const& pOrientation)
{
	Vector3f lN = pOrientation.Vector() * (2.f / pOrientation.SquaredNorm());
	Vector3f lW = lN * pOrientation.W;
	Vector3f lX = lN * pOrientation.X;

	float lYY = pOrientation.Y * lN.Y;
	float lYZ = pOrientation.Y * lN.Z;
	float lZZ = pOrientation.Z * lN.Z;

	M[0] = { 1.f - (lYY + lZZ), lX.Y + lW.Z, lX.Z - lW.Y, 0.f };
	M[1] = { lX.Y - lW.Z, 1.f - (lX.X + lZZ), lYZ + lW.X, 0.f };
	M[2] = { lX.Z + lW.Y, lYZ - lW.X, 1.f - (lX.X + lYY), 0.f };
}

void Matrix4f::SetPosition(Vector3f const& pPosition)
{
	M[3].AsVector3Ref() = pPosition;
}

const Matrix4f Matrix4f::Id = {
	{ 1, 0, 0, 0 },
	{ 0, 1, 0, 0 },
	{ 0, 0, 1, 0 },
	{ 0, 0, 0, 1 }
};

void Matrix4f::SetPerspectiveProjection(float pLeft, float pRight, float pBottom, float pTop, float pNear, float pFar)
{
	M[0] = { (2.f * pNear) / (pRight - pLeft), 0.f, 0.f, 0.f };
	M[1] = { 0.f, (2.f * pNear) / (pTop - pBottom), 0.f, 0.f };
	M[2] = { (pRight + pLeft) / (pRight - pLeft), (pTop + pBottom) / (pTop - pBottom), (pFar + pNear) / (-(pFar - pNear)), -1.f };
	M[3] = { 0.f, 0.f, (2.f * pFar * pNear) / (-(pFar - pNear)), 0.f };
}

void Matrix4f::SetOrthographicProjection(float pLeft, float pRight, float pBottom, float pTop, float pNear, float pFar)
{
	M[0] = { 2.f / (pRight - pLeft), 0.f, 0.f, 0.f };
	M[1] = { 0.f, 2.f / (pTop - pBottom), 0.f, 0.f };
	M[2] = { 0.f, 0.f, -2.f / (pFar - pNear), 0.f };
	M[3] = { -(pRight + pLeft) / (pRight - pLeft), -(pTop + pBottom) / (pTop - pBottom), -(pFar + pNear) / (pFar - pNear), 1.f };
}

void Matrix4f::SetScale(float const pScale)
{
	float lScaleX = pScale;
	float lScaleY = pScale;
	float lScaleZ = pScale;

	M[0][0] *= lScaleX;
	M[0][1] *= lScaleY;
	M[0][2] *= lScaleZ;
	M[1][0] *= lScaleX;
	M[1][1] *= lScaleY;
	M[1][2] *= lScaleZ;
	M[2][0] *= lScaleX;
	M[2][1] *= lScaleY;
	M[2][2] *= lScaleZ;
}

Quaternionf Matrix4f::GetQuaternion() const
{//Hugues' method based on Shepperd algorithm
	Quaternionf lReturnQuat;
	float lTrace = M[0][0] + M[1][1] + M[2][2];
	float lCommonTerm;

	if (lTrace > 0.f)
	{
		lCommonTerm = sqrtf(lTrace + 1.f);
		lReturnQuat.W = lCommonTerm * .5f;
		lCommonTerm = .5f / lCommonTerm;

		lReturnQuat.X = (M[1][2] - M[2][1]) * lCommonTerm;
		lReturnQuat.Y = (M[2][0] - M[0][2]) * lCommonTerm;
		lReturnQuat.Z = (M[0][1] - M[1][0]) * lCommonTerm;
	}
	else
	{
		float lMaxValue = M[0][0];
		uint32_t lI0 = 0;
		if (M[1][1] > lMaxValue)
		{
			lMaxValue = M[1][1];
			lI0 = 1;
		}

		if (M[2][2] > lMaxValue)
		{
			lI0 = 2;
		}

		uint32_t lI1 = (lI0 + 1) % 3;
		uint32_t lI2 = (lI0 + 2) % 3;

		lCommonTerm = sqrtf((M[lI0][lI0] - (M[lI1][lI1] + M[lI2][lI2])) + 1.f);
		lReturnQuat[lI0] = lCommonTerm * .5f;
		lCommonTerm = .5f / lCommonTerm;

		lReturnQuat[lI1] = (M[lI0][lI1] + M[lI1][lI0]) * lCommonTerm;
		lReturnQuat[lI2] = (M[lI0][lI2] + M[lI2][lI0]) * lCommonTerm;
			
		lReturnQuat.W = (M[lI1][lI2] - M[lI2][lI1]) * lCommonTerm;
		
	}
	return lReturnQuat;
}

Vector3f Matrix4f::GetEuler() const
{//TODO:
	float lSinRoll, lCosRoll, lSinYaw, lCosYaw;
	float lSinPitch = -M[2][0];
	float lCosPitch = sqrtf(1 - lSinPitch * lSinPitch);

	if (fabs(lCosPitch) > 0.0000001f)
	{
		lSinRoll = M[2][1] / lCosPitch;
		lCosRoll = M[2][2] / lCosPitch;
		lSinYaw = M[1][0] / lCosPitch;
		lCosYaw = M[0][0] / lCosPitch;
	}
	else
	{
		lSinRoll = -M[1][2];
		lCosRoll = M[1][1];
		lSinYaw = 0;
		lCosYaw = 1;
	}

	return Vector3f(atan2f(lSinYaw, lCosYaw), atan2f(lSinPitch, lCosPitch), atan2f(lSinRoll, lCosRoll));
}

Quaternionf& Matrix4f::operator[](unsigned int pIndex)
{ 
	return M[pIndex];
}

const Quaternionf& Matrix4f::operator[](unsigned int pIndex) const
{
	return M[pIndex];
}

Matrix4f operator*(Matrix4f const& pLeftMatrix, Matrix4f const& pRightMatrix)
{
	Matrix4f lResultMatrix;

	for (int lRow = 0; lRow < 4; ++lRow)
	{
		for (int lCol = 0; lCol < 4; lCol++)
		{
			lResultMatrix[lCol][lRow] = 0.f;

			for (int i = 0; i < 4; i++)
			{
				lResultMatrix[lCol][lRow] += pLeftMatrix[i][lRow] * pRightMatrix[lCol][i];
			}
		}
	}

	return lResultMatrix;
}
