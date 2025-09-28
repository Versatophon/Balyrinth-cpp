#include "ContiguousMazeDrawer.h"

#include <Shape.h>

ContiguousMazeDrawer::ContiguousMazeDrawer(GeometryContainer& pGeometryContainer) :
    MazeDrawer(pGeometryContainer)
{
}

void ContiguousMazeDrawer::DrawEdge(DrawRectangleFunc pFunction, uint32_t pNodeIndex0, uint32_t pNodeIndex1, float pDepth)
{
    const Vector2f& lNode0Position = mNodePositions[pNodeIndex0];

    Vector3f lSpaceDelta = mShape->GetUnitSpaceDelta(pNodeIndex0, pNodeIndex1);

    Vector3f lNodeNormalizedPos0 = mShape->GetNodeNormalizedPosition(pNodeIndex0);
    Vector3f lNodeNormalizedPos1 = mShape->GetNodeNormalizedPosition(pNodeIndex1);

    Vector2f lInitialNode0Position = { lNodeNormalizedPos0.X , lNodeNormalizedPos0.Y };

    //Since the space delta is from node 0 to node 1, we need to compensate it with opposited delta
    Vector2f lInitialNode1Position = { (lNodeNormalizedPos1.X - lSpaceDelta.X * mSpaceSize.X),
                                   (lNodeNormalizedPos1.Y - lSpaceDelta.Y * mSpaceSize.Y) };

    Vector2f lNode1Position = lNode0Position + (lInitialNode1Position - lInitialNode0Position);

    mNodePositions[pNodeIndex1] = lNode1Position;

    Vector3f lMin{ std::min(lNode0Position.X, lNode1Position.X) - mLineOffset, std::min(lNode0Position.Y, lNode1Position.Y) - mLineOffset, pDepth };
    Vector3f lMax{ std::max(lNode0Position.X, lNode1Position.X) + mLineOffset, std::max(lNode0Position.Y, lNode1Position.Y) + mLineOffset, pDepth };
    (this->*pFunction)(lMin, lMax);
}
