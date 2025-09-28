#include "Vector3f.h"

#include <cmath>

#include "Trigonometry.h"

float Vector3f::Norm() const
{
    return sqrtf(X*X + Y*Y+ Z*Z);
}

Vector3f Vector3f::Normalized() const
{
    float lNorm = Norm();

    if (lNorm != 0)
    {
        return {X/lNorm, Y/lNorm, Z/lNorm};
    }

    return { 0, 0, 0 };
}

void Vector3f::Normalize()
{
    *this = Normalized();
}

Vector3f Vector3f::CrossProduct(const Vector3f& pOther) const
{
    return Vector3f((Y * pOther.Z) - (Z * pOther.Y),
                    (Z * pOther.X) - (X * pOther.Z),
                    (X * pOther.Y) - (Y * pOther.X));
}

float Vector3f::DotProduct(const Vector3f& pOther) const
{
    return (X * pOther.X
          + Y * pOther.Y
          + Z * pOther.Z);
}

float Vector3f::Angle2D(const Vector3f& pFrom, const Vector3f& pTo)
{
    float lAngle = atan2f(pTo.Y, pTo.X) - atan2f(pFrom.Y, pFrom.X);

    if (lAngle > PI)
    {
        lAngle -= 2.f * PI;
    }

    if (lAngle < -PI)
    {
        lAngle += 2.f * PI;
    }

    return lAngle;
}

float Vector3f::Angle(Vector3f const& pFrom, Vector3f const& pTo)
{
    float lNormProduct = pFrom.Norm() * pTo.Norm();

    if (lNormProduct < EPSILON)
    {
        return 0.f;
    }

    return acosf(pFrom.DotProduct(pTo) / lNormProduct);
}

float& Vector3f::operator[](unsigned int pIndex)
{ 
    return (&X)[pIndex];
}

const float& Vector3f::operator[](unsigned int pIndex) const
{ 
    return (&X)[pIndex];
}

const Vector3f Vector3f::Zero = { 0, 0, 0 };
const Vector3f Vector3f::XUnit = { 1, 0, 0 };
const Vector3f Vector3f::YUnit = { 0, 1, 0 };
const Vector3f Vector3f::ZUnit = { 0, 0, 1 };

Vector3f& Vector3f::operator+=(const Vector3f& pOther)
{
    X += pOther.X;
    Y += pOther.Y;
    Z += pOther.Z;
    return *this;
}

Vector3f& Vector3f::operator-=(const Vector3f& pOther)
{
    X += pOther.X;
    Y += pOther.Y;
    Z += pOther.Z;
    return *this;
}

Vector3f operator+(const Vector3f& pLeft, const Vector3f& pRight)
{
    return { pLeft.X + pRight.X, pLeft.Y + pRight.Y, pLeft.Z + pRight.Z };
}

Vector3f operator-(const Vector3f& pLeft, const Vector3f& pRight)
{
    return { pLeft.X - pRight.X, pLeft.Y - pRight.Y, pLeft.Z - pRight.Z };
}

Vector3f operator*(float pLeft, const Vector3f& pRight)
{
    return { pLeft * pRight.X, pLeft * pRight.Y, pLeft * pRight.Z };
}

Vector3f operator*(const Vector3f& pLeft, float pRight)
{
    return pRight * pLeft;
}

Vector3f operator/(float pLeft, const Vector3f& pRight)
{
    return { pLeft / pRight.X, pLeft / pRight.Y, pLeft / pRight.Z };
}

Vector3f operator/(const Vector3f& pLeft, float pRight)
{
    return { pLeft.X/pRight, pLeft.Y/pRight, pLeft.Z/pRight };
}

bool operator==(const Vector3f& pLeft, const Vector3f& pRight)
{
    return pLeft.X == pRight.X && pLeft.Y == pRight.Y && pLeft.Z == pRight.Z;
}

bool operator!=(const Vector3f& pLeft, const Vector3f& pRight)
{
    return !(pLeft == pRight);
}

std::ostream& operator<<(std::ostream& pStream, const Vector3f& pVector)
{
    pStream << "V( " << pVector.X << " , " << pVector.Y << " ," << pVector.Z << " )";
    return pStream;
}
