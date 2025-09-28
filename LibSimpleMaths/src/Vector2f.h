#pragma once

#include "SimpleMathsApi.h"

#include <cstdint>

struct SIMPLE_MATHS_API Vector2f
{
    float X;
    float Y;

    Vector2f& operator+=(const Vector2f& pOther);
    Vector2f& operator-=(const Vector2f& pOther);

    float Norm();
};

SIMPLE_MATHS_API Vector2f operator+(const Vector2f& pLeft, const Vector2f& pRight);

SIMPLE_MATHS_API Vector2f operator-(const Vector2f& pLeft, const Vector2f& pRight);

