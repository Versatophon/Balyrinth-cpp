#include "Vector2i.h"

#include <cmath>

Vector2i& Vector2i::operator+=(const Vector2i& pOther)
{
    X += pOther.X;
    Y += pOther.Y;
    return *this;
}

Vector2i& Vector2i::operator-=(const Vector2i& pOther)
{
    X += pOther.X;
    Y += pOther.Y;
    return *this;
}

Vector2i operator+(const Vector2i& pLeft, const Vector2i& pRight)
{
    return { pLeft.X + pRight.X, pLeft.Y + pRight.Y };
}

Vector2i operator-(const Vector2i& pLeft, const Vector2i& pRight)
{
    return { pLeft.X - pRight.X, pLeft.Y - pRight.Y };
}


#if 0
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
#endif
