#include "Quaternionf.h"

#include "Vector3f.h"
#include "Matrix4f.h"

#include "Trigonometry.h"

Quaternionf::Quaternionf(float pX, float pY, float pZ, float pW) : X(pX), Y(pY), Z(pZ), W(pW)
{
}

Quaternionf::Quaternionf(const Quaternionf& pOther) : X(pOther.X), Y(pOther.Y), Z(pOther.Z), W(pOther.W)
{
}

Quaternionf::Quaternionf(const Vector3f& pAxis, const float pRadians)
{
	Vector3f lNormalizedVector = pAxis.Normalized();

	if (lNormalizedVector.Norm() < EPSILON) {
		*this = Quaternionf::Id;
		return;
	}

	float lSinA = sinf(pRadians / 2.f);

	X = lSinA * lNormalizedVector.X;
	Y = lSinA * lNormalizedVector.Y;
	Z = lSinA * lNormalizedVector.Z;
	W = cosf(pRadians / 2.f);
}

Quaternionf::Quaternionf(const Vector3f& pVector) :Quaternionf(pVector.X, pVector.Y, pVector.Z, 0.f)
{
}

Quaternionf::Quaternionf(const Vector3f& pFrom, const Vector3f& pTo)
{
	Vector3f lNormalizedFrom = pFrom.Normalized();
	Vector3f lNormalizedTo = pTo.Normalized();
	Vector3f lRotationAxis = lNormalizedFrom.CrossProduct(lNormalizedTo);

	float lCrossProductMagnitude = sqrtf(lRotationAxis.DotProduct(lRotationAxis));

	if (lCrossProductMagnitude > 1.f)
		lCrossProductMagnitude = 1.f;

	float lTheta = asinf(lCrossProductMagnitude);
	float lThetaComplement = PI - lTheta;

	if (lNormalizedFrom.DotProduct(lNormalizedTo) < 0.0)
	{
		lTheta = lThetaComplement;
		lThetaComplement = PI - lTheta;
	}

	if (lTheta > EPSILON)
	{
		if (lThetaComplement < EPSILON)
		{
			if (((lNormalizedFrom.Y * lNormalizedFrom.Y) + (lNormalizedFrom.Z * lNormalizedFrom.Z)) >= EPSILON)
			{
				lRotationAxis.X = 0.f;
				lRotationAxis.Y = lNormalizedFrom.Z;
				lRotationAxis.Z = -lNormalizedFrom.Y;
			}
			else
			{
				lRotationAxis.X = lRotationAxis.Y = 0.f;
				lRotationAxis.Z = 1.f;
			}
		}

		lRotationAxis = lRotationAxis.Normalized();
		*this = Quaternionf(lRotationAxis, lTheta).Normalized();
	}
}

Quaternionf::~Quaternionf()
{
}

void Quaternionf::Normalize()
{
	*this = Normalized();
}

Quaternionf Quaternionf::Normalized() const
{
	return *this / Norm();
}

float Quaternionf::SquaredNorm() const
{
	return X * X + Y * Y + Z * Z + W * W;
}

float Quaternionf::Norm() const
{
	return sqrtf(SquaredNorm());
}

Quaternionf Quaternionf::Conjugated() const
{
	return Quaternionf(-X, -Y, -Z, W);
}

Quaternionf Quaternionf::Inverted() const
{
	return Conjugated().Normalized();
}

Quaternionf Quaternionf::Exponentiated() const
{
	float lTheta = Vector().Norm();
	float lScale = 1.f;

	if (lTheta > EPSILON)
	{
		lScale = sinf(lTheta) / lTheta;
	}

	return Quaternionf(lScale * X, lScale * Y, lScale * Z, cosf(lTheta));
}

Quaternionf Quaternionf::Logarithmated() const
{
	float lScale = Vector().Norm();
	float lTheta = atan2f(lScale, W);

	if (lScale > 0.f)
	{
		lScale = lTheta / lScale;
	}

	return Quaternionf(lScale * X, lScale * Y, lScale * Z, 0.f);
}

Vector3f Quaternionf::Vector() const
{
	return Vector3f(X, Y, Z);
}

Quaternionf Quaternionf::Transform(Quaternionf const& pQuaternion) const
{
	return ((*this) * pQuaternion * Inverted());
}

Vector3f Quaternionf::Transform(Vector3f const& pVector) const
{
	return Transform(Quaternionf(pVector)).Vector();
}

float* Quaternionf::Array()
{ 
	return &X;
}

const float* Quaternionf::Array() const
{ 
	return &X;
}

float& Quaternionf::operator[](unsigned int pIndex)
{ 
	return Array()[pIndex];
}

const float& Quaternionf::operator[](unsigned int pIndex) const
{
	return Array()[pIndex];
}

Vector3f& Quaternionf::AsVector3Ref()
{
	return *((Vector3f*)this);
}

Quaternionf Quaternionf::operator*=(Quaternionf const& pQuaternion)
{
	(*this) = (*this) * pQuaternion;
	return *this;
}

Quaternionf Quaternionf::operator*=(float pFactor)
{
	X *= pFactor;
	Y *= pFactor;
	Z *= pFactor;
	W *= pFactor;
	return *this;
}

Quaternionf Quaternionf::GetLocalOrientationFrom(const Quaternionf& pFrom) const
{
	return (Inverted() * pFrom).Inverted();
}

Quaternionf Quaternionf::FromEuler(const Vector3f& pEuler)
{
	const int lMinorAxis = 1;
	const int lMiddleAxis = 0;
	const int lMajorAxis = 2;

	float lCosMin = cos(pEuler[lMinorAxis] * 0.5f);
	float lSinMin = sin(pEuler[lMinorAxis] * 0.5f);
	float lCosMid = cos(pEuler[lMiddleAxis] * 0.5f);
	float lSinMid = sin(pEuler[lMiddleAxis] * 0.5f);
	float lCosMaj = cos(pEuler[lMajorAxis] * 0.5f);
	float lSinMaj = sin(pEuler[lMajorAxis] * 0.5f);

	//Quaternion multiplication is not commutative, some signs must be opposited
	float lDirectOpposition = (((3 + (int32_t(lMiddleAxis)) - (int32_t(lMajorAxis))) % 3) == 1) ? -1.f : 1.f;

	Quaternionf lReturnValue;
	
	lReturnValue[lMiddleAxis] = lCosMin * lSinMid * lCosMaj + lDirectOpposition * lSinMin * lCosMid * lSinMaj;
	lReturnValue[lMinorAxis] = lSinMin * lCosMid * lCosMaj - lDirectOpposition * lCosMin * lSinMid * lSinMaj;
	lReturnValue[lMajorAxis] = lCosMin * lCosMid * lSinMaj - lDirectOpposition * lSinMin * lSinMid * lCosMaj;
	lReturnValue.W = lCosMin * lCosMid * lCosMaj + lDirectOpposition * lSinMin * lSinMid * lSinMaj;

	return lReturnValue;
}

Vector3f Quaternionf::ToEuler() const
{
	const int lMinorAxis = 1;
	const int lMiddleAxis = 0;
	const int lMajorAxis = 2;

	Vector3f lResult;

	Quaternionf lNormalized = Normalized();

	//Quaternion multiplication is not commutative, some signs must be opposited
	float lDirectOpposition = (((3 + (int32_t(lMiddleAxis)) - (int32_t(lMajorAxis))) % 3) == 1) ? -1.f : 1.f;

	float lSinMiddleAxis = 2 * (lNormalized.W * lNormalized[lMiddleAxis] - lDirectOpposition * lNormalized[lMajorAxis] * lNormalized[lMinorAxis]);
	if (std::fabs(lSinMiddleAxis) >= (1.f - (4 * FLT_EPSILON)))//Unable to differentiate Minor and Major axis
	{
		float lSign = std::copysign(1.f, lSinMiddleAxis);

		//Set Minor Axis Rotation to 0
		lResult[lMinorAxis] = 0;

		//Set Middle Axis Rotation to PI/2 or -PI/2
		lResult[lMiddleAxis] = lSign * PI / 2.f;

		//Major Axis Rotation
		lResult[lMajorAxis] = lSign * 2.f * atan2f(-lDirectOpposition * lNormalized[lMinorAxis], lNormalized.W);
	}
	else
	{
		//Minor Axis Rotation
		float lSinMinCosMid = 2.f * (lNormalized.W * lNormalized[lMinorAxis] + lDirectOpposition * lNormalized[lMiddleAxis] * lNormalized[lMajorAxis]);
		float lCosMinCosMid = 1.f - 2.f * (lNormalized[lMinorAxis] * lNormalized[lMinorAxis] + lNormalized[lMiddleAxis] * lNormalized[lMiddleAxis]);
		lResult[lMinorAxis] = std::atan2f(lSinMinCosMid, lCosMinCosMid);

		//Middle Axis Rotation
		lResult[lMiddleAxis] = std::asinf(lSinMiddleAxis);

		//Major Axis Rotation
		float lSinMajCosMid = 2 * (lNormalized.W * lNormalized[lMajorAxis] + lDirectOpposition * lNormalized[lMinorAxis] * lNormalized[lMiddleAxis]);
		float lCosMajCosMid = 1 - 2 * (lNormalized[lMiddleAxis] * lNormalized[lMiddleAxis] + lNormalized[lMajorAxis] * lNormalized[lMajorAxis]);
		lResult[lMajorAxis] = std::atan2(lSinMajCosMid, lCosMajCosMid);
	}

	return lResult;
}

void Quaternionf::SetLocalOrientationFrom(const Quaternionf& pFrom, const Quaternionf& pQuaternion)
{
	*this = pFrom * pQuaternion;
}

const Quaternionf Quaternionf::Id = { 0, 0, 0, 1 };
const Quaternionf Quaternionf::XUnit = { 1, 0, 0, 0 };
const Quaternionf Quaternionf::YUnit = { 0, 1, 0, 0 };
const Quaternionf Quaternionf::ZUnit = { 0, 0, 1, 0 };
const Quaternionf Quaternionf::WUnit = Quaternionf::Id;

Quaternionf operator/(const Quaternionf& pQuaternion, const float& pScalar)
{
	return Quaternionf(pQuaternion.X / pScalar, pQuaternion.Y / pScalar, pQuaternion.Z / pScalar, pQuaternion.W / pScalar);
}

Quaternionf operator*(const Quaternionf& pLeft, const Quaternionf& pRight)
{
	return Quaternionf(
		pLeft.W * pRight.X + pLeft.X * pRight.W + pLeft.Y * pRight.Z - pLeft.Z * pRight.Y,
		pLeft.W * pRight.Y + pLeft.Y * pRight.W + pLeft.Z * pRight.X - pLeft.X * pRight.Z,
		pLeft.W * pRight.Z + pLeft.Z * pRight.W + pLeft.X * pRight.Y - pLeft.Y * pRight.X,
		pLeft.W * pRight.W - pLeft.X * pRight.X - pLeft.Y * pRight.Y - pLeft.Z * pRight.Z);
}

Quaternionf operator+(const Quaternionf& pLeft, Quaternionf const& pRight)
{
	return Quaternionf(pLeft.X + pRight.X, pLeft.Y + pRight.Y, pLeft.Z + pRight.Z, pLeft.W + pRight.W);
}

Quaternionf operator-(const Quaternionf& pLeft, Quaternionf const& pRight)
{
	return Quaternionf(pLeft.X - pRight.X, pLeft.Y - pRight.Y, pLeft.Z - pRight.Z, pLeft.W - pRight.W);
}

Quaternionf Slerp(const Quaternionf& pFrom, const Quaternionf& pTo, const float pPosition)
{
	float lStartScale, lEndScale;
	Quaternionf lFrom = pFrom;

	float lCosOmega = pFrom.X * pTo.X + pFrom.Y * pTo.Y + pFrom.Z * pTo.Z + pFrom.W * pTo.W;

	if (lCosOmega < 0.f)
	{
		lCosOmega *= -1.f;
		lFrom *= -1.f;
	}

	if ((1.f + lCosOmega) > EPSILON)
	{
		if ((1.f - lCosOmega) > EPSILON)
		{
			float lOmega = acosf(lCosOmega);
			float lSinOmega = sinf(lOmega);
			lStartScale = sinf((1.f - pPosition) * lOmega) / lSinOmega;
			lEndScale = sinf(pPosition * lOmega) / lSinOmega;
		}
		else
		{
			lStartScale = 1.f - pPosition;
			lEndScale = pPosition;
		}

		return lStartScale * lFrom + lEndScale * pTo;
	}
	else
	{
		lStartScale = sinf((.5f - pPosition) * PI);
		lEndScale = sinf(pPosition * PI);

		return
		{
			lStartScale * lFrom.X - lEndScale * lFrom.Y,
			lStartScale * lFrom.Y + lEndScale * lFrom.X,
			lStartScale * lFrom.Z - lEndScale * lFrom.W,
			lFrom.Z
		};
	}
}

bool operator==(const Quaternionf& pLeft, const Quaternionf& pRight)
{
	return pLeft.W == pRight.W && pLeft.X == pRight.X && pLeft.Y == pRight.Y && pLeft.Z == pRight.Z;
}

bool operator!=(const Quaternionf& pLeft, const Quaternionf& pRight)
{
	return !(pLeft == pRight);
}

std::ostream& operator<<(std::ostream& pStream, const Quaternionf& pQuaternion)
{
	pStream << "Q( " << pQuaternion.X << " ; " << pQuaternion.Y << " ; " << pQuaternion.Z << " ; " << pQuaternion.W << " )";
	return pStream;
}
