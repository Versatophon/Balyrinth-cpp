#pragma once

#include "MazeDrawer.h"

class ContiguousMazeDrawer :public MazeDrawer
{
public:
    ContiguousMazeDrawer(GeometryContainer& pGeometryContainer);

private:
    void DrawEdge(DrawRectangleFunc pFunction, uint32_t pNodeIndex0, uint32_t pNodeIndex1, float pDepth) override;
};
