#pragma once

#include "SimpleMathsApi.h"

#include <cstdint>
#include <iostream>

struct SIMPLE_MATHS_API Vector3f
{
    float X = 0;
    float Y = 0;
    float Z = 0;

    float Norm() const;
    Vector3f Normalized() const;

    void Normalize();

    Vector3f CrossProduct(const Vector3f& pOther) const;
    float DotProduct(const Vector3f& pOther) const;
    
    Vector3f& operator+=(const Vector3f& pOther);
    Vector3f& operator-=(const Vector3f& pOther);
  
    static float Angle2D(const Vector3f& pFrom, const Vector3f& pTo);
    static float Angle(const Vector3f& pFrom, const Vector3f& pTo);

    float& operator[] (unsigned int pIndex);
    const float& operator[] (unsigned int pIndex) const;

    static const Vector3f Zero;
    static const Vector3f XUnit;
    static const Vector3f YUnit;
    static const Vector3f ZUnit;
};

SIMPLE_MATHS_API std::ostream& operator<<(std::ostream& pStream, const Vector3f& pVector);

SIMPLE_MATHS_API Vector3f operator+(const Vector3f& pLeft, const Vector3f& pRight);
SIMPLE_MATHS_API Vector3f operator-(const Vector3f& pLeft, const Vector3f& pRight);
SIMPLE_MATHS_API Vector3f operator*(float pLeft, const Vector3f& pRight);
SIMPLE_MATHS_API Vector3f operator*(const Vector3f& pLeft, float pRight);
SIMPLE_MATHS_API Vector3f operator/(float pLeft, const Vector3f& pRight);
SIMPLE_MATHS_API Vector3f operator/(const Vector3f& pLeft, float pRight);

SIMPLE_MATHS_API bool operator==(const Vector3f& pLeft, const Vector3f& pRight);
SIMPLE_MATHS_API bool operator!=(const Vector3f& pLeft, const Vector3f& pRight);
