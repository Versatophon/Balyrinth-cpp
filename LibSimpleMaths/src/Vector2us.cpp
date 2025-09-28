#include "Vector2us.h"

#include <cmath>

Vector2us& Vector2us::operator+=(const Vector2us& pOther)
{
    X += pOther.X;
    Y += pOther.Y;
    return *this;
}

Vector2us& Vector2us::operator-=(const Vector2us& pOther)
{
    X += pOther.X;
    Y += pOther.Y;
    return *this;
}

Vector2us operator+(const Vector2us& pLeft, const Vector2us& pRight)
{
    return { uint16_t(pLeft.X + pRight.X), uint16_t(pLeft.Y + pRight.Y) };
}

Vector2us operator-(const Vector2us& pLeft, const Vector2us& pRight)
{
    return { uint16_t(pLeft.X - pRight.X), uint16_t(pLeft.Y - pRight.Y) };
}
