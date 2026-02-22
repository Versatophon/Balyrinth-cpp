#include "MazeDrawer.h"

#include <Shape.h>

#include <cfloat>

#include "MazeGeometyParameters.h"

#define DEBUG_MULT .001f

#define TWO_DIV_SQR3 1.1547005383792515290182975610039

static Vector3f P0{ 1.f, 0.5 * 1.1547005383792515290182975610039 , 0.f };
static Vector3f P1{ 0.f, 1.f * 1.1547005383792515290182975610039 , 0.f };
static Vector3f P2{ -1.f, 0.5 * 1.1547005383792515290182975610039 , 0.f };
static Vector3f P3{ -1.f, -0.5 * 1.1547005383792515290182975610039 , 0.f };
static Vector3f P4{ 0.f, -1.f * 1.1547005383792515290182975610039 , 0.f };
static Vector3f P5{ 1.f, -0.5 * 1.1547005383792515290182975610039 , 0.f };

class NodeShapeProvider
{
public:
    inline size_t GetVertexCountPerNode() const
    {
        return mVertexCountPerNode;
    }

    virtual void DrawNode(std::vector<float>& pContainer, const Vector2f& pNodePosition, const MazeGeometryParameters& pMazeGeometryParameters) = 0;


protected:
    size_t mVertexCountPerNode;
};

void AddAARect(std::vector<float>& pContainer, const Vector3f& pMin, const Vector3f& pMax)
{
    pContainer.push_back(pMin.X); pContainer.push_back(pMin.Y); pContainer.push_back(pMin.Z);
    pContainer.push_back(pMax.X); pContainer.push_back(pMin.Y); pContainer.push_back(pMin.Z);
    pContainer.push_back(pMax.X); pContainer.push_back(pMax.Y); pContainer.push_back(pMin.Z);
    pContainer.push_back(pMax.X); pContainer.push_back(pMax.Y); pContainer.push_back(pMin.Z);
    pContainer.push_back(pMin.X); pContainer.push_back(pMax.Y); pContainer.push_back(pMin.Z);
    pContainer.push_back(pMin.X); pContainer.push_back(pMin.Y); pContainer.push_back(pMin.Z);
}

//TODO::Add Grid on which you handle a topology 

#if 1
class SquareNodeShapeProvider :public NodeShapeProvider
{
public:
    SquareNodeShapeProvider()
    {
        mVertexCountPerNode = 6;
    }

    void DrawNode(std::vector<float>& pContainer, const Vector2f& pNodePosition, const MazeGeometryParameters& pMazeGeometryParameters) override
    {
        Vector3f lMin{ pNodePosition.X - pMazeGeometryParameters.PointWidth / 2, pNodePosition.Y - pMazeGeometryParameters.PointWidth / 2, -2 * DEBUG_MULT };
        Vector3f lMax{ pNodePosition.X + pMazeGeometryParameters.PointWidth / 2, pNodePosition.Y + pMazeGeometryParameters.PointWidth / 2, -2 * DEBUG_MULT };
        AddAARect(pContainer, lMin, lMax);
    }

private:
};

class HexagonNodeShapeProvider :public NodeShapeProvider
{
public:
    HexagonNodeShapeProvider()
    {
        mVertexCountPerNode = 12;
    }

    void DrawNode(std::vector<float>& pContainer, const Vector2f& pNodePosition, const MazeGeometryParameters& pMazeGeometryParameters) override
    {
        Vector3f lPosition{ pNodePosition.X, pNodePosition.Y, -2 * DEBUG_MULT };
        //Vector3f lMax{ pNodePosition.X + pMazeGeometryParameters.PointWidth / 2, pNodePosition.Y + pMazeGeometryParameters.PointWidth / 2, -2 * DEBUG_MULT };
        AddHexagon(pContainer, lPosition, pMazeGeometryParameters.PointWidth / 2);
    }

private:
    void AddHexagon(std::vector<float>& pContainer, const Vector3f& pCenter, const float pRadius)
    {
        pContainer.push_back(pCenter.X + pRadius * P5.X); pContainer.push_back(pCenter.Y + pRadius * P5.Y); pContainer.push_back(pCenter.Z);
        pContainer.push_back(pCenter.X + pRadius * P0.X); pContainer.push_back(pCenter.Y + pRadius * P0.Y); pContainer.push_back(pCenter.Z);
        pContainer.push_back(pCenter.X + pRadius * P1.X); pContainer.push_back(pCenter.Y + pRadius * P1.Y); pContainer.push_back(pCenter.Z);

        pContainer.push_back(pCenter.X + pRadius * P1.X); pContainer.push_back(pCenter.Y + pRadius * P1.Y); pContainer.push_back(pCenter.Z);
        pContainer.push_back(pCenter.X + pRadius * P2.X); pContainer.push_back(pCenter.Y + pRadius * P2.Y); pContainer.push_back(pCenter.Z);
        pContainer.push_back(pCenter.X + pRadius * P5.X); pContainer.push_back(pCenter.Y + pRadius * P5.Y); pContainer.push_back(pCenter.Z);

        pContainer.push_back(pCenter.X + pRadius * P4.X); pContainer.push_back(pCenter.Y + pRadius * P4.Y); pContainer.push_back(pCenter.Z);
        pContainer.push_back(pCenter.X + pRadius * P5.X); pContainer.push_back(pCenter.Y + pRadius * P5.Y); pContainer.push_back(pCenter.Z);
        pContainer.push_back(pCenter.X + pRadius * P2.X); pContainer.push_back(pCenter.Y + pRadius * P2.Y); pContainer.push_back(pCenter.Z);

        pContainer.push_back(pCenter.X + pRadius * P2.X); pContainer.push_back(pCenter.Y + pRadius * P2.Y); pContainer.push_back(pCenter.Z);
        pContainer.push_back(pCenter.X + pRadius * P3.X); pContainer.push_back(pCenter.Y + pRadius * P3.Y); pContainer.push_back(pCenter.Z);
        pContainer.push_back(pCenter.X + pRadius * P4.X); pContainer.push_back(pCenter.Y + pRadius * P4.Y); pContainer.push_back(pCenter.Z);
    }
};
#endif

MazeDrawer::MazeDrawer(GeometryContainer& pGeometryContainer, ShapeMode pShapeMode) :
    mGeometryContainer(pGeometryContainer),
    mNodePositions(mGeometryContainer.GetNodePositions()),
    mMazeGeometryParameters(mGeometryContainer.GetMazeGeometryParameters()),
    mVerticesToAdd(mGeometryContainer.GetVerticesToAdd()),
    mForNodesVerticesToAdd(mGeometryContainer.GetForNodesVerticesToAdd()),
    mForNodesLut(mGeometryContainer.GetForNodesLut()),
    mForNodesCount(mGeometryContainer.GetForNodesCount()),
    mForPathVerticesToAdd(mGeometryContainer.GetForPathVerticesToAdd()),
    mShapeMode(pShapeMode),
    //mNodeShapeProvider(new SquareNodeShapeProvider())
    mNodeShapeProvider(new HexagonNodeShapeProvider())
{
}

void MazeDrawer::SetShapeMode(ShapeMode pShapeMode)
{
    mShapeMode = pShapeMode;
}

size_t MazeDrawer::GetVertexCountPerNode() const
{
    return mNodeShapeProvider->GetVertexCountPerNode();
}

void MazeDrawer::AddFirstNode(uint32_t pNodeIndex)
{
    InitStaticVariables();

    //clean
    mGeometryContainer.CleanupGeometry();

    mNodePositions.resize(mMazeGeometryParameters.Width * mMazeGeometryParameters.Height, Vector2f{ FLT_MAX , FLT_MAX });

    for (size_t j = 0; j < mMazeGeometryParameters.Height; ++j)
    {
        for (size_t i = 0; i < mMazeGeometryParameters.Width; ++i)
        {
            mNodePositions[j * mMazeGeometryParameters.Width + i] = Vector2f{ float(i), float(j) };
        }
    }

    DrawNode(pNodeIndex, true);
}

void MazeDrawer::AddEdge(uint32_t pNodeIndex0, uint32_t pNodeIndex1)
{
    DrawEdge(mVerticesToAdd, pNodeIndex0, pNodeIndex1, -3 * DEBUG_MULT);

    DrawNode(pNodeIndex1);

    size_t lVertexCountPerNode = mNodeShapeProvider->GetVertexCountPerNode();
    for (size_t i = 0; i < lVertexCountPerNode; ++i)
    {
        mForNodesCount[mForNodesLut[pNodeIndex0] * lVertexCountPerNode + i]++;
        mForNodesCount[mForNodesLut[pNodeIndex1] * lVertexCountPerNode + i]++;
    }
}

void MazeDrawer::UpdaterProcessCompleted(uint32_t pPathLength, const uint32_t* pPathIndices)
{
    for (uint32_t i = 1; i < pPathLength; ++i)
    {
        DrawEdge(mForPathVerticesToAdd, pPathIndices[i - 1], pPathIndices[i], -1 * DEBUG_MULT);
    }
}

void MazeDrawer::InitStaticVariables()
{
    mLineOffset = mMazeGeometryParameters.LineWidth / 2.f;
    mShape = mGeometryContainer.GetShape();
    mSpaceSize = mShape->GetSpaceSize();
    mHalfWidth = (1.f / 2.f) + mLineOffset;
}

void MazeDrawer::DrawNode(uint32_t pIndex, bool pInit)
{
    if (pInit)
    {
        mLastLutIndex = 0;
    }

    const Vector2f& lNodePosition = mNodePositions[pIndex];
    mForNodesLut[pIndex] = mLastLutIndex++;
    mNodeShapeProvider->DrawNode(mForNodesVerticesToAdd, lNodePosition, mMazeGeometryParameters);
}

void MazeDrawer::DrawEdge(std::vector<float>& pContainer, uint32_t pNodeIndex0, uint32_t pNodeIndex1, float pDepth)
{
    switch (mShapeMode)
    {
    case ShapeMode::Contiguous:
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
        AddAARect(pContainer, lMin, lMax);
    }
        break;
    case ShapeMode::Shape:
    {
        const Vector2f& lNode0Position = mNodePositions[pNodeIndex0];
        const Vector2f& lNode1Position = mNodePositions[pNodeIndex1];

        Vector3f lSpaceDelta = mShape->GetUnitSpaceDelta(pNodeIndex0, pNodeIndex1);

        if ((abs(lSpaceDelta.X) + abs(lSpaceDelta.Y) + abs(lSpaceDelta.Z)) == 0)
        {
            Vector3f lMin{ std::min(lNode0Position.X, lNode1Position.X) - mLineOffset, std::min(lNode0Position.Y, lNode1Position.Y) - mLineOffset, pDepth };
            Vector3f lMax{ std::max(lNode0Position.X, lNode1Position.X) + mLineOffset, std::max(lNode0Position.Y, lNode1Position.Y) + mLineOffset, pDepth };
            AddAARect(pContainer, lMin, lMax);
        }
        else if (lSpaceDelta.X < 0 && true)
        {
            Vector3f lNodeNormalizedPos1 = mShape->GetNodeNormalizedPosition(pNodeIndex1);
            Vector2f lVirtualalNode0Position = { (-lSpaceDelta.X * mSpaceSize.X + lNodeNormalizedPos1.X),
                                             (-lSpaceDelta.Y * mSpaceSize.Y + lNodeNormalizedPos1.Y) };


            Vector3f lMin{ lNode0Position.X - mLineOffset, lNode0Position.Y - mLineOffset, pDepth };
            Vector3f lMax{ lMin.X + mHalfWidth, lVirtualalNode0Position.Y + mLineOffset, pDepth };
            AddAARect(pContainer, lMin, lMax);

            lMin.X -= mMazeGeometryParameters.Width - mHalfWidth;
            lMax.X -= mMazeGeometryParameters.Width - mHalfWidth;
            AddAARect(pContainer, lMin, lMax);
        }
        else if (lSpaceDelta.X > 0 && true)
        {
            Vector3f lNodeNormalizedPos1 = mShape->GetNodeNormalizedPosition(pNodeIndex1);
            Vector2f lVirtualNode0Position = { -lSpaceDelta.X * mSpaceSize.X + lNodeNormalizedPos1.X,
                                           -lSpaceDelta.Y * mSpaceSize.Y + lNodeNormalizedPos1.Y };

            Vector3f lMax{ lNode0Position.X + mLineOffset, lNode0Position.Y + mLineOffset, pDepth };
            Vector3f lMin{ (lMax.X + lVirtualNode0Position.X - mLineOffset) / 2, lVirtualNode0Position.Y - mLineOffset, pDepth };
            AddAARect(pContainer, lMin, lMax);

            lMin.X += mMazeGeometryParameters.Width - mHalfWidth;
            lMax.X += mMazeGeometryParameters.Width - mHalfWidth;
            AddAARect(pContainer, lMin, lMax);
        }
        else if (lSpaceDelta.Y < 0 && true)
        {
            Vector3f lNodeNormalizedPos1 = mShape->GetNodeNormalizedPosition(pNodeIndex1);
            Vector2f lVirtualalNode0Position = { -lSpaceDelta.X * mSpaceSize.X + lNodeNormalizedPos1.X,
                                             -lSpaceDelta.Y * mSpaceSize.Y + lNodeNormalizedPos1.Y };

            Vector3f lMin{ lNode0Position.X - mLineOffset, lNode0Position.Y - mLineOffset, pDepth };
            Vector3f lMax{ lVirtualalNode0Position.X + mLineOffset, (lMin.Y + lVirtualalNode0Position.Y + mLineOffset) / 2, pDepth };
            AddAARect(pContainer, lMin, lMax);

            lMin.Y -= mMazeGeometryParameters.Height - mHalfWidth;
            lMax.Y -= mMazeGeometryParameters.Height - mHalfWidth;
            AddAARect(pContainer, lMin, lMax);
        }
        else if (lSpaceDelta.Y > 0 && true)
        {
            Vector3f lNodeNormalizedPos1 = mShape->GetNodeNormalizedPosition(pNodeIndex1);
            Vector2f lVirtualNode0Position = { -lSpaceDelta.X * mSpaceSize.X + lNodeNormalizedPos1.X,
                                           -lSpaceDelta.Y * mSpaceSize.Y + lNodeNormalizedPos1.Y };

            Vector3f lMax{ lNode0Position.X + mLineOffset, lNode0Position.Y + mLineOffset, pDepth };
            Vector3f lMin{ lVirtualNode0Position.X - mLineOffset, (lMax.Y + lVirtualNode0Position.Y - mLineOffset) / 2, pDepth };
            AddAARect(pContainer, lMin, lMax);

            lMin.Y += mMazeGeometryParameters.Height - mHalfWidth;
            lMax.Y += mMazeGeometryParameters.Height - mHalfWidth;
            AddAARect(pContainer, lMin, lMax);
        }
    }
        break;
    }
}
