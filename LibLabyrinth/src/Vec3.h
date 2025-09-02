#pragma once

#include "LabyrinthApi.h"

#include <cstdint>

struct LABYRINTH_API Vec3
{
    float X;
    float Y;
    float Z;
    
    Vec3& operator+=(const Vec3& pOther);
    Vec3& operator-=(const Vec3& pOther);
};

LABYRINTH_API Vec3 operator+(const Vec3& pLeft, const Vec3& pRight);
LABYRINTH_API Vec3 operator-(const Vec3& pLeft, const Vec3& pRight);

struct LABYRINTH_API Vec3i
{
    int32_t X;
    int32_t Y;
    int32_t Z;

    Vec3i& operator+=(const Vec3i& pOther);
    Vec3i& operator-=(const Vec3i& pOther);
};

LABYRINTH_API Vec3i operator+(const Vec3i& pLeft, const Vec3i& pRight);
LABYRINTH_API Vec3i operator-(const Vec3i& pLeft, const Vec3i& pRight);
