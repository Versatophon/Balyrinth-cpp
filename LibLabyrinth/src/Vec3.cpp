#include "Vec3.h"

Vec3& Vec3::operator+=(const Vec3& pOther)
{
    X += pOther.X;
    Y += pOther.Y;
    Z += pOther.Z;
    return *this;
}

Vec3& Vec3::operator-=(const Vec3& pOther)
{
    X += pOther.X;
    Y += pOther.Y;
    Z += pOther.Z;
    return *this;
}

Vec3 operator+(const Vec3& pLeft, const Vec3& pRight)
{
    return { pLeft.X + pRight.X, pLeft.Y + pRight.Y, pLeft.Z + pRight.Z };
}

Vec3 operator-(const Vec3& pLeft, const Vec3& pRight)
{
    return { pLeft.X - pRight.X, pLeft.Y - pRight.Y, pLeft.Z - pRight.Z };
}

Vec3i& Vec3i::operator+=(const Vec3i& pOther)
{
    X += pOther.X;
    Y += pOther.Y;
    Z += pOther.Z;
    return *this;
}

Vec3i& Vec3i::operator-=(const Vec3i& pOther)
{
    X += pOther.X;
    Y += pOther.Y;
    Z += pOther.Z;
    return *this;
}

Vec3i operator+(const Vec3i& pLeft, const Vec3i& pRight)
{
    return { pLeft.X + pRight.X, pLeft.Y + pRight.Y, pLeft.Z + pRight.Z };
}

Vec3i operator-(const Vec3i& pLeft, const Vec3i& pRight)
{
    return { pLeft.X - pRight.X, pLeft.Y - pRight.Y, pLeft.Z - pRight.Z };
}
