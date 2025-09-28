#include "MazeDrawer.h"

#include <Shape.h>

#define DEBUG_MULT 1.f

MazeDrawer::MazeDrawer(GeometryContainer& pGeometryContainer) :
    mGeometryContainer(pGeometryContainer),
    mNodePositions(mGeometryContainer.GetNodePositions()),
    mMazeGeometryParameters(mGeometryContainer.GetMazeGeometryParameters()),
    mVerticesToAdd(mGeometryContainer.GetVerticesToAdd()),
    mForNodesVerticesToAdd(mGeometryContainer.GetForNodesVerticesToAdd()),
    mForNodesLut(mGeometryContainer.GetForNodesLut()),
    mForNodesCount(mGeometryContainer.GetForNodesCount()),
    mForPathVerticesToAdd(mGeometryContainer.GetForPathVerticesToAdd())
{
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
    DrawEdge(&MazeDrawer::DrawAAEdge, pNodeIndex0, pNodeIndex1, -3 * DEBUG_MULT);

    DrawNode(pNodeIndex1);

    for (size_t i = 0; i < 6; ++i)
    {
        mForNodesCount[mForNodesLut[pNodeIndex0] * 6 + i]++;
        mForNodesCount[mForNodesLut[pNodeIndex1] * 6 + i]++;
    }
}

void MazeDrawer::UpdaterProcessCompleted(uint32_t pPathLength, const uint32_t* pPathIndices)
{
    for (uint32_t i = 1; i < pPathLength; ++i)
    {
        DrawEdge(&MazeDrawer::DrawAAPath, pPathIndices[i - 1], pPathIndices[i], -1 * DEBUG_MULT);
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

    Vector3f lMin{ lNodePosition.X - mMazeGeometryParameters.PointWidth / 2, lNodePosition.Y - mMazeGeometryParameters.PointWidth / 2, -2 * DEBUG_MULT };
    Vector3f lMax{ lNodePosition.X + mMazeGeometryParameters.PointWidth / 2, lNodePosition.Y + mMazeGeometryParameters.PointWidth / 2, -2 * DEBUG_MULT };
    DrawAANode(lMin, lMax);
}

void AddAARect(std::vector<float>& pContainer, const Vector3f& pMin, const Vector3f& pMax)
{
    pContainer.push_back(pMin.X); pContainer.push_back(pMin.Y); pContainer.push_back(pMin.Z);
    pContainer.push_back(pMax.X); pContainer.push_back(pMin.Y); pContainer.push_back(pMin.Z);
    pContainer.push_back(pMax.X); pContainer.push_back(pMax.Y); pContainer.push_back(pMin.Z);
    pContainer.push_back(pMax.X); pContainer.push_back(pMax.Y); pContainer.push_back(pMin.Z);
    pContainer.push_back(pMin.X); pContainer.push_back(pMax.Y); pContainer.push_back(pMin.Z);
    pContainer.push_back(pMin.X); pContainer.push_back(pMin.Y); pContainer.push_back(pMin.Z);
}

void MazeDrawer::DrawAANode(const Vector3f& pMin, const Vector3f& pMax)
{
    AddAARect(mForNodesVerticesToAdd, pMin, pMax);
}

void MazeDrawer::DrawAAEdge(const Vector3f& pMin, const Vector3f& pMax)
{
    AddAARect(mVerticesToAdd, pMin, pMax);
}

void MazeDrawer::DrawAAPath(const Vector3f& pMin, const Vector3f& pMax)
{
    AddAARect(mForPathVerticesToAdd, pMin, pMax);
}
