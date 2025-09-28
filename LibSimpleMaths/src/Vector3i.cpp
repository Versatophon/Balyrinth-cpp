#include "Vector3i.h"

#include <cmath>

Vector3i& Vector3i::operator+=(const Vector3i& pOther)
{
    X += pOther.X;
    Y += pOther.Y;
    Z += pOther.Z;
    return *this;
}

Vector3i& Vector3i::operator-=(const Vector3i& pOther)
{
    X += pOther.X;
    Y += pOther.Y;
    Z += pOther.Z;
    return *this;
}

Vector3i operator+(const Vector3i& pLeft, const Vector3i& pRight)
{
    return { pLeft.X + pRight.X, pLeft.Y + pRight.Y, pLeft.Z + pRight.Z };
}

Vector3i operator-(const Vector3i& pLeft, const Vector3i& pRight)
{
    return { pLeft.X - pRight.X, pLeft.Y - pRight.Y, pLeft.Z - pRight.Z };
}
