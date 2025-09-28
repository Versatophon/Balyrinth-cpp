#include "Vector2f.h"

#include <cmath>

Vector2f& Vector2f::operator+=(const Vector2f& pOther)
{
    X += pOther.X;
    Y += pOther.Y;
    return *this;
}

Vector2f& Vector2f::operator-=(const Vector2f& pOther)
{
    X += pOther.X;
    Y += pOther.Y;
    return *this;
}

float Vector2f::Norm()
{
    return sqrtf(X * X + Y * Y);
}

Vector2f operator+(const Vector2f& pLeft, const Vector2f& pRight)
{
    return { pLeft.X + pRight.X, pLeft.Y + pRight.Y };
}

Vector2f operator-(const Vector2f& pLeft, const Vector2f& pRight)
{
    return { pLeft.X - pRight.X, pLeft.Y - pRight.Y };
}
