#pragma once

#include <cstddef>//size_t

struct MazeGeometryParameters
{
    size_t Width = 10;
    size_t Height = 10;

    float NodeWidth = .5f;
    float EdgeWidth = .25f;
};
