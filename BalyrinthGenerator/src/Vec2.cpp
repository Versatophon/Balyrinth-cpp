#include "Vec2.h"

#include <cmath>

Vec2& Vec2::operator+=(const Vec2& pOther)
{
    X += pOther.X;
    Y += pOther.Y;
    return *this;
}

Vec2& Vec2::operator-=(const Vec2& pOther)
{
    X += pOther.X;
    Y += pOther.Y;
    return *this;
}

float Vec2::Norm()
{
    return sqrtf(X * X + Y * Y);
}

Vec2 operator+(const Vec2& pLeft, const Vec2& pRight)
{
    return { pLeft.X + pRight.X, pLeft.Y + pRight.Y };
}

Vec2 operator-(const Vec2& pLeft, const Vec2& pRight)
{
    return { pLeft.X - pRight.X, pLeft.Y - pRight.Y };
}



Vec2i& Vec2i::operator+=(const Vec2i& pOther)
{
    X += pOther.X;
    Y += pOther.Y;
    return *this;
}

Vec2i& Vec2i::operator-=(const Vec2i& pOther)
{
    X += pOther.X;
    Y += pOther.Y;
    return *this;
}

Vec2i operator+(const Vec2i& pLeft, const Vec2i& pRight)
{
    return { pLeft.X + pRight.X, pLeft.Y + pRight.Y };
}

Vec2i operator-(const Vec2i& pLeft, const Vec2i& pRight)
{
    return { pLeft.X - pRight.X, pLeft.Y - pRight.Y };
}



Vec2u& Vec2u::operator+=(const Vec2u& pOther)
{
    X += pOther.X;
    Y += pOther.Y;
    return *this;
}

Vec2u& Vec2u::operator-=(const Vec2u& pOther)
{
    X += pOther.X;
    Y += pOther.Y;
    return *this;
}

Vec2u operator+(const Vec2u& pLeft, const Vec2u& pRight)
{
    return { uint16_t(pLeft.X + pRight.X), uint16_t(pLeft.Y + pRight.Y) };
}

Vec2u operator-(const Vec2u& pLeft, const Vec2u& pRight)
{
    return { uint16_t(pLeft.X - pRight.X), uint16_t(pLeft.Y - pRight.Y) };
}
