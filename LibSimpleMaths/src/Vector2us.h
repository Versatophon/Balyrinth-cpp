#pragma once

#include "SimpleMathsApi.h"

#include <cstdint>

struct SIMPLE_MATHS_API Vector2us
{
    uint16_t X;
    uint16_t Y;

    Vector2us& operator+=(const Vector2us& pOther);
    Vector2us& operator-=(const Vector2us& pOther);
};

SIMPLE_MATHS_API Vector2us operator+(const Vector2us& pLeft, const Vector2us& pRight);

SIMPLE_MATHS_API Vector2us operator-(const Vector2us& pLeft, const Vector2us& pRight);
