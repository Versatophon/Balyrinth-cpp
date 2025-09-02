#pragma once

struct FMat4
{
    float _00;
    float _10;
    float _20;
    float _30;
    float _01;
    float _11;
    float _21;
    float _31;
    float _02;
    float _12;
    float _22;
    float _32;
    float _03;
    float _13;
    float _23;
    float _33;

    static const FMat4 Id;
};

const FMat4 FMat4::Id =
{
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};

inline FMat4 GenPerspective(float pLeft, float pRight, float pBottom, float pTop, float pNear, float pFar)
{
    return {
        (2.f * pNear) / (pRight - pLeft), 0.f, 0.f, 0.f,
        0.f, (2.f * pNear) / (pTop - pBottom), 0.f, 0.f,
        (pRight + pLeft) / (pRight - pLeft), (pTop + pBottom) / (pTop - pBottom), (pFar + pNear) / (-(pFar - pNear)), -1.f,
        0.f, 0.f, (2.f * pFar * pNear) / (-(pFar - pNear)), 0.f };
}


inline FMat4 GenOrthographic(float pLeft, float pRight, float pBottom, float pTop, float pNear, float pFar)
{
    return {
        2.f / (pRight - pLeft), 0.f, 0.f, 0.f,
        0.f, 2.f / (pTop - pBottom), 0.f, 0.f,
        0.f, 0.f, -2.f / (pFar - pNear), 0.f,
        -(pRight + pLeft) / (pRight - pLeft), -(pTop + pBottom) / (pTop - pBottom), -(pFar + pNear) / (pFar - pNear), 1.f };
}
