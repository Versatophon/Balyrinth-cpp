#pragma once

#include <cstddef>//size_t

struct MazeGeometryParameters
{
    int32_t Width = 10;
    int32_t Height = 10;

    float NodeWidth = .5f;
    float EdgeWidth = .25f;
};
