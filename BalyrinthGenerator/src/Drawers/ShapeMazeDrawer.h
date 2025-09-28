#pragma once

#include <Labyrinth.h>

#include "MazeDrawer.h"

class ShapeMazeDrawer :public MazeDrawer
{
public:
    ShapeMazeDrawer(GeometryContainer& pGeometryContainer);

private:
    void DrawEdge(DrawRectangleFunc pFunction, uint32_t pNodeIndex0, uint32_t pNodeIndex1, float pDepth) override;
};
