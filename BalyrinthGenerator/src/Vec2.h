#pragma once

#include <cstdint>

struct Vec2
{
    float X;
    float Y;

    Vec2& operator+=(const Vec2& pOther);
    Vec2& operator-=(const Vec2& pOther);

    float Norm();
};

Vec2 operator+(const Vec2& pLeft, const Vec2& pRight);

Vec2 operator-(const Vec2& pLeft, const Vec2& pRight);

struct Vec2i
{
    int32_t X;
    int32_t Y;

    Vec2i& operator+=(const Vec2i& pOther);
    Vec2i& operator-=(const Vec2i& pOther);
};

Vec2i operator+(const Vec2i& pLeft, const Vec2i& pRight);

Vec2i operator-(const Vec2i& pLeft, const Vec2i& pRight);

struct Vec2u
{
    uint16_t X;
    uint16_t Y;

    Vec2u& operator+=(const Vec2u& pOther);
    Vec2u& operator-=(const Vec2u& pOther);
};

Vec2u operator+(const Vec2u& pLeft, const Vec2u& pRight);

Vec2u operator-(const Vec2u& pLeft, const Vec2u& pRight);

struct Recti
{
    Vec2i Position;
    Vec2i Size;
};
