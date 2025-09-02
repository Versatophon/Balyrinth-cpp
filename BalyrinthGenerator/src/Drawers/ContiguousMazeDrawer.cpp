#include "ContiguousMazeDrawer.h"

#include <Shape.h>

ContiguousMazeDrawer::ContiguousMazeDrawer(GeometryContainer& pGeometryContainer) :
    MazeDrawer(pGeometryContainer)
{
}

void ContiguousMazeDrawer::DrawEdge(DrawRectangleFunc pFunction, uint32_t pNodeIndex0, uint32_t pNodeIndex1)
{
    const Vec2& lNode0Position = mNodePositions[pNodeIndex0];

    Vec3i lSpaceDelta = mShape->GetUnitSpaceDelta(pNodeIndex0, pNodeIndex1);

    Vec3i lNodeNormalizedPos0 = mShape->GetNodeNormalizedPosition(pNodeIndex0);
    Vec3i lNodeNormalizedPos1 = mShape->GetNodeNormalizedPosition(pNodeIndex1);

    Vec2 lInitialNode0Position = { lNodeNormalizedPos0.X * mMazeGeometryParameters.CellWidth, lNodeNormalizedPos0.Y * mMazeGeometryParameters.CellWidth };
    Vec2 lInitialNode1Position = { (lSpaceDelta.X * mSpaceSize.X + lNodeNormalizedPos1.X) * mMazeGeometryParameters.CellWidth,
                                    (lSpaceDelta.Y * mSpaceSize.Y + lNodeNormalizedPos1.Y) * mMazeGeometryParameters.CellWidth };

    Vec2 lNode1Position = lNode0Position + (lInitialNode1Position - lInitialNode0Position);

    mNodePositions[pNodeIndex1] = lNode1Position;

    Vec3 lMin{ std::min(lNode0Position.X, lNode1Position.X) - mLineOffset, std::min(lNode0Position.Y, lNode1Position.Y) - mLineOffset, -3 };
    Vec3 lMax{ std::max(lNode0Position.X, lNode1Position.X) + mLineOffset, std::max(lNode0Position.Y, lNode1Position.Y) + mLineOffset, -3 };
    (this->*pFunction)(lMin, lMax);
}
