#pragma once

const float EPSILON = 1e-12;
const float PI = 3.14159265358979323846f;

inline float D2R(float pValue) { return pValue * PI / 180.f; }
inline float R2D(float pValue) { return pValue * 180.f / PI; }
