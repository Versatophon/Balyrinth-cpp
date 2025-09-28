#include "ShapeMazeDrawer.h"

#include <Shape.h>

ShapeMazeDrawer::ShapeMazeDrawer(GeometryContainer& pGeometryContainer) :
    MazeDrawer(pGeometryContainer)
{
}

void ShapeMazeDrawer::DrawEdge(DrawRectangleFunc pFunction, uint32_t pNodeIndex0, uint32_t pNodeIndex1, float pDepth)
{
    const Vector2f& lNode0Position = mNodePositions[pNodeIndex0];
    const Vector2f& lNode1Position = mNodePositions[pNodeIndex1];

    Vector3f lSpaceDelta = mShape->GetUnitSpaceDelta(pNodeIndex0, pNodeIndex1);

    if ((abs(lSpaceDelta.X) + abs(lSpaceDelta.Y) + abs(lSpaceDelta.Z)) == 0)
    {
        Vector3f lMin{ std::min(lNode0Position.X, lNode1Position.X) - mLineOffset, std::min(lNode0Position.Y, lNode1Position.Y) - mLineOffset, pDepth };
        Vector3f lMax{ std::max(lNode0Position.X, lNode1Position.X) + mLineOffset, std::max(lNode0Position.Y, lNode1Position.Y) + mLineOffset, pDepth };
        (this->*pFunction)(lMin, lMax);
    }
    else if (lSpaceDelta.X < 0 && true)
    {
        Vector3f lNodeNormalizedPos1 = mShape->GetNodeNormalizedPosition(pNodeIndex1);
        Vector2f lVirtualalNode0Position = { (-lSpaceDelta.X * mSpaceSize.X + lNodeNormalizedPos1.X),
                                         (-lSpaceDelta.Y * mSpaceSize.Y + lNodeNormalizedPos1.Y) };


        Vector3f lMin{ lNode0Position.X - mLineOffset, lNode0Position.Y - mLineOffset, pDepth };
        Vector3f lMax{ lMin.X + mHalfWidth, lVirtualalNode0Position.Y + mLineOffset, pDepth };
        (this->*pFunction)(lMin, lMax);

        lMin.X -= mMazeGeometryParameters.Width - mHalfWidth;
        lMax.X -= mMazeGeometryParameters.Width - mHalfWidth;
        (this->*pFunction)(lMin, lMax);
    }
    else if (lSpaceDelta.X > 0 && true)
    {
        Vector3f lNodeNormalizedPos1 = mShape->GetNodeNormalizedPosition(pNodeIndex1);
        Vector2f lVirtualNode0Position = { -lSpaceDelta.X * mSpaceSize.X + lNodeNormalizedPos1.X,
                                       -lSpaceDelta.Y * mSpaceSize.Y + lNodeNormalizedPos1.Y };

        Vector3f lMax{ lNode0Position.X + mLineOffset, lNode0Position.Y + mLineOffset, pDepth };
        Vector3f lMin{ (lMax.X + lVirtualNode0Position.X - mLineOffset) / 2, lVirtualNode0Position.Y - mLineOffset, pDepth };
        (this->*pFunction)(lMin, lMax);

        lMin.X += mMazeGeometryParameters.Width - mHalfWidth;
        lMax.X += mMazeGeometryParameters.Width - mHalfWidth;
        (this->*pFunction)(lMin, lMax);
    }
    else if (lSpaceDelta.Y < 0 && true)
    {
        Vector3f lNodeNormalizedPos1 = mShape->GetNodeNormalizedPosition(pNodeIndex1);
        Vector2f lVirtualalNode0Position = { -lSpaceDelta.X * mSpaceSize.X + lNodeNormalizedPos1.X,
                                         -lSpaceDelta.Y * mSpaceSize.Y + lNodeNormalizedPos1.Y };

        Vector3f lMin{ lNode0Position.X - mLineOffset, lNode0Position.Y - mLineOffset, pDepth };
        Vector3f lMax{ lVirtualalNode0Position.X + mLineOffset, (lMin.Y + lVirtualalNode0Position.Y + mLineOffset) / 2, pDepth };
        (this->*pFunction)(lMin, lMax);

        lMin.Y -= mMazeGeometryParameters.Height - mHalfWidth;
        lMax.Y -= mMazeGeometryParameters.Height - mHalfWidth;
        (this->*pFunction)(lMin, lMax);
    }
    else if (lSpaceDelta.Y > 0 && true)
    {
        Vector3f lNodeNormalizedPos1 = mShape->GetNodeNormalizedPosition(pNodeIndex1);
        Vector2f lVirtualNode0Position = { -lSpaceDelta.X * mSpaceSize.X + lNodeNormalizedPos1.X,
                                       -lSpaceDelta.Y * mSpaceSize.Y + lNodeNormalizedPos1.Y };

        Vector3f lMax{ lNode0Position.X + mLineOffset, lNode0Position.Y + mLineOffset, pDepth };
        Vector3f lMin{ lVirtualNode0Position.X - mLineOffset, (lMax.Y + lVirtualNode0Position.Y - mLineOffset) / 2, pDepth };
        (this->*pFunction)(lMin, lMax);

        lMin.Y += mMazeGeometryParameters.Height - mHalfWidth;
        lMax.Y += mMazeGeometryParameters.Height - mHalfWidth;
        (this->*pFunction)(lMin, lMax);
    }
}
