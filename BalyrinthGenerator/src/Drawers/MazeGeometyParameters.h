#pragma once

#include <cstddef>//size_t

struct MazeGeometryParameters
{
    size_t Width = 10;
    size_t Height = 10;

    float PointWidth = .5f;
    float LineWidth = .25f;
};
