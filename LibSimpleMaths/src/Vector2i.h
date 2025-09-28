#pragma once

#include "SimpleMathsApi.h"

#include <cstdint>

struct SIMPLE_MATHS_API Vector2i
{
    union
    {
        struct
        {
            int32_t X;
            int32_t Y;
        };

        struct
        {
            int32_t Width;
            int32_t Height;
        };
    };

    Vector2i& operator+=(const Vector2i& pOther);
    Vector2i& operator-=(const Vector2i& pOther);
};

SIMPLE_MATHS_API Vector2i operator+(const Vector2i& pLeft, const Vector2i& pRight);

SIMPLE_MATHS_API Vector2i operator-(const Vector2i& pLeft, const Vector2i& pRight);
