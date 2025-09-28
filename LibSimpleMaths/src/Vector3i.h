#pragma once

#include "SimpleMathsApi.h"

#include <cstdint>
#include <iostream>

struct SIMPLE_MATHS_API Vector3i
{
    int32_t X;
    int32_t Y;
    int32_t Z;

    Vector3i& operator+=(const Vector3i& pOther);
    Vector3i& operator-=(const Vector3i& pOther);
};

SIMPLE_MATHS_API Vector3i operator+(const Vector3i& pLeft, const Vector3i& pRight);
SIMPLE_MATHS_API Vector3i operator-(const Vector3i& pLeft, const Vector3i& pRight);
