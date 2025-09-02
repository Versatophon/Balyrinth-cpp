#include "ShapeMazeDrawer.h"

#include <Shape.h>

ShapeMazeDrawer::ShapeMazeDrawer(GeometryContainer& pGeometryContainer) :
    MazeDrawer(pGeometryContainer)
{
}

void ShapeMazeDrawer::DrawEdge(DrawRectangleFunc pFunction, uint32_t pNodeIndex0, uint32_t pNodeIndex1)
{
    const Vec2& lNode0Position = mNodePositions[pNodeIndex0];
    const Vec2& lNode1Position = mNodePositions[pNodeIndex1];

    Vec3i lSpaceDelta = mShape->GetUnitSpaceDelta(pNodeIndex0, pNodeIndex1);

    if ((abs(lSpaceDelta.X) + abs(lSpaceDelta.Y) + abs(lSpaceDelta.Z)) == 0)
    {
        Vec3 lMin{ std::min(lNode0Position.X, lNode1Position.X) - mLineOffset, std::min(lNode0Position.Y, lNode1Position.Y) - mLineOffset, -3 };
        Vec3 lMax{ std::max(lNode0Position.X, lNode1Position.X) + mLineOffset, std::max(lNode0Position.Y, lNode1Position.Y) + mLineOffset, -3 };
        (this->*pFunction)(lMin, lMax);
    }
    else if (lSpaceDelta.X > 0 && true)
    {
        Vec3i lNodeNormalizedPos1 = mShape->GetNodeNormalizedPosition(pNodeIndex1);
        Vec2 lVirtualalNode0Position = { (lSpaceDelta.X * mSpaceSize.X + lNodeNormalizedPos1.X) * mMazeGeometryParameters.CellWidth,
                                         (lSpaceDelta.Y * mSpaceSize.Y + lNodeNormalizedPos1.Y) * mMazeGeometryParameters.CellWidth };


        Vec3 lMin{ lNode0Position.X - mLineOffset, lNode0Position.Y - mLineOffset, -3 };
        Vec3 lMax{ lMin.X + mHalfWidth, lVirtualalNode0Position.Y + mLineOffset, -3 };
        (this->*pFunction)(lMin, lMax);

        lMin.X -= (mMazeGeometryParameters.Width * mMazeGeometryParameters.CellWidth) - mHalfWidth;
        lMax.X -= (mMazeGeometryParameters.Width * mMazeGeometryParameters.CellWidth) - mHalfWidth;
        (this->*pFunction)(lMin, lMax);
    }
    else if (lSpaceDelta.X < 0 && true)
    {
        Vec3i lNodeNormalizedPos1 = mShape->GetNodeNormalizedPosition(pNodeIndex1);
        Vec2 lVirtualNode0Position = { (lSpaceDelta.X * mSpaceSize.X + lNodeNormalizedPos1.X) * mMazeGeometryParameters.CellWidth,
                                       (lSpaceDelta.Y * mSpaceSize.Y + lNodeNormalizedPos1.Y) * mMazeGeometryParameters.CellWidth };

        Vec3 lMax{ lNode0Position.X + mLineOffset, lNode0Position.Y + mLineOffset, -3 };
        Vec3 lMin{ (lMax.X + lVirtualNode0Position.X - mLineOffset) / 2, lVirtualNode0Position.Y - mLineOffset, -3 };
        (this->*pFunction)(lMin, lMax);

        lMin.X += (mMazeGeometryParameters.Width * mMazeGeometryParameters.CellWidth) - mHalfWidth;
        lMax.X += (mMazeGeometryParameters.Width * mMazeGeometryParameters.CellWidth) - mHalfWidth;
        (this->*pFunction)(lMin, lMax);
    }
    else if (lSpaceDelta.Y > 0 && true)
    {
        Vec3i lNodeNormalizedPos1 = mShape->GetNodeNormalizedPosition(pNodeIndex1);
        Vec2 lVirtualalNode0Position = { (lSpaceDelta.X * mSpaceSize.X + lNodeNormalizedPos1.X) * mMazeGeometryParameters.CellWidth,
                                         (lSpaceDelta.Y * mSpaceSize.Y + lNodeNormalizedPos1.Y) * mMazeGeometryParameters.CellWidth };

        Vec3 lMin{ lNode0Position.X - mLineOffset, lNode0Position.Y - mLineOffset, -3 };
        Vec3 lMax{ lVirtualalNode0Position.X + mLineOffset, (lMin.Y + lVirtualalNode0Position.Y + mLineOffset) / 2, -3 };
        (this->*pFunction)(lMin, lMax);

        lMin.Y -= (mMazeGeometryParameters.Height * mMazeGeometryParameters.CellWidth) - mHalfWidth;
        lMax.Y -= (mMazeGeometryParameters.Height * mMazeGeometryParameters.CellWidth) - mHalfWidth;
        (this->*pFunction)(lMin, lMax);
    }
    else if (lSpaceDelta.Y < 0 && true)
    {
        Vec3i lNodeNormalizedPos1 = mShape->GetNodeNormalizedPosition(pNodeIndex1);
        Vec2 lVirtualNode0Position = { (lSpaceDelta.X * mSpaceSize.X + lNodeNormalizedPos1.X) * mMazeGeometryParameters.CellWidth,
                                       (lSpaceDelta.Y * mSpaceSize.Y + lNodeNormalizedPos1.Y) * mMazeGeometryParameters.CellWidth };

        Vec3 lMax{ lNode0Position.X + mLineOffset, lNode0Position.Y + mLineOffset, -3 };
        Vec3 lMin{ lVirtualNode0Position.X - mLineOffset, (lMax.Y + lVirtualNode0Position.Y - mLineOffset) / 2, -3 };
        (this->*pFunction)(lMin, lMax);

        lMin.Y += (mMazeGeometryParameters.Height * mMazeGeometryParameters.CellWidth) - mHalfWidth;
        lMax.Y += (mMazeGeometryParameters.Height * mMazeGeometryParameters.CellWidth) - mHalfWidth;
        (this->*pFunction)(lMin, lMax);
    }
}
