#include "MazeDrawer.h"

#include <Shape.h>

#include <cfloat>

#include "MazeGeometyParameters.h"

#define DEBUG_MULT .001f

#define SQRT3BY2 .86602540378443864676372317075294f
#define SQRT3 1.7320508075688772935274463415059f
#define TWOBYSQRT3 1.1547005383792515290182975610039f

static Vector3f P0{ SQRT3, 1.f, 0.f };
static Vector3f P1{ 0.f, 2.f, 0.f };
static Vector3f P2{ -SQRT3, 1.f, 0.f };
static Vector3f P3{ -SQRT3, -1.f, 0.f };
static Vector3f P4{ 0.f, -2.f, 0.f };
static Vector3f P5{ SQRT3, -1.f, 0.f };

class NodeShapeProvider
{
public:
    inline size_t GetVertexCountPerNode() const
    {
        return mVertexCountPerNode;
    }

    virtual void DrawNode(std::vector<float>& pContainer, const Vector3f& pNodePosition, const MazeGeometryParameters& pMazeGeometryParameters, bool pEvenRow) = 0;


protected:
    size_t mVertexCountPerNode;
};

class EdgeShapeProvider
{
public:
    virtual void DrawEdge(std::vector<float>& pContainer, uint32_t pNodeIndex0, uint32_t pNodeIndex1, float pDepth) = 0;
};

//TODO::Add Grid on which you handle a topology 

#if 1
class InvalidNodeShapeProvider:public NodeShapeProvider
{
public:
    InvalidNodeShapeProvider()
    {
        mVertexCountPerNode = 0;
    }

    void DrawNode(std::vector<float>& pContainer, const Vector3f& pNodePosition, const MazeGeometryParameters& pMazeGeometryParameters, bool pEvenRow) override
    {
    }
};

class TriangleNodeShapeProvider:public NodeShapeProvider
{
public:
    TriangleNodeShapeProvider()
    {
        mVertexCountPerNode = 3;
    }

    void DrawNode(std::vector<float>& pContainer, const Vector3f& pNodePosition, const MazeGeometryParameters& pMazeGeometryParameters, bool pEvenRow) override
    {
        Vector3f lPosition{ pNodePosition.X, pNodePosition.Y, pNodePosition.Z -2 * DEBUG_MULT };

        AddTriangle(pContainer, lPosition, pMazeGeometryParameters.PointWidth / 2, pEvenRow);
    }

private:
    void AddTriangle(std::vector<float>& pContainer, const Vector3f& pCenter, const float pRadius, bool pEvenRow)
    {
        if (pEvenRow)
        {
            pContainer.push_back(pCenter.X + pRadius * P1.X); pContainer.push_back(pCenter.Y + pRadius * P1.Y); pContainer.push_back(pCenter.Z);
            pContainer.push_back(pCenter.X + pRadius * P3.X); pContainer.push_back(pCenter.Y + pRadius * P3.Y); pContainer.push_back(pCenter.Z);
            pContainer.push_back(pCenter.X + pRadius * P5.X); pContainer.push_back(pCenter.Y + pRadius * P5.Y); pContainer.push_back(pCenter.Z);
        }
        else
        {
            pContainer.push_back(pCenter.X + pRadius * P0.X); pContainer.push_back(pCenter.Y + pRadius * P0.Y); pContainer.push_back(pCenter.Z);
            pContainer.push_back(pCenter.X + pRadius * P2.X); pContainer.push_back(pCenter.Y + pRadius * P2.Y); pContainer.push_back(pCenter.Z);
            pContainer.push_back(pCenter.X + pRadius * P4.X); pContainer.push_back(pCenter.Y + pRadius * P4.Y); pContainer.push_back(pCenter.Z);
        }
    }
};

class SquareNodeShapeProvider:public NodeShapeProvider
{
public:
    SquareNodeShapeProvider()
    {
        mVertexCountPerNode = 6;
    }

    void DrawNode(std::vector<float>& pContainer, const Vector3f& pNodePosition, const MazeGeometryParameters& pMazeGeometryParameters, bool pEvenRow) override
    {
        Vector3f lMin{ pNodePosition.X - pMazeGeometryParameters.PointWidth / 2, pNodePosition.Y - pMazeGeometryParameters.PointWidth / 2, pNodePosition.Z -2 * DEBUG_MULT };
        Vector3f lMax{ pNodePosition.X + pMazeGeometryParameters.PointWidth / 2, pNodePosition.Y + pMazeGeometryParameters.PointWidth / 2, pNodePosition.Z -2 * DEBUG_MULT };
        AddAARect(pContainer, lMin, lMax);
    }

private:
    void AddAARect(std::vector<float>& pContainer, const Vector3f& pMin, const Vector3f& pMax)
    {
        pContainer.push_back(pMin.X); pContainer.push_back(pMin.Y); pContainer.push_back(pMin.Z);
        pContainer.push_back(pMax.X); pContainer.push_back(pMin.Y); pContainer.push_back(pMin.Z);
        pContainer.push_back(pMax.X); pContainer.push_back(pMax.Y); pContainer.push_back(pMin.Z);
        pContainer.push_back(pMax.X); pContainer.push_back(pMax.Y); pContainer.push_back(pMin.Z);
        pContainer.push_back(pMin.X); pContainer.push_back(pMax.Y); pContainer.push_back(pMin.Z);
        pContainer.push_back(pMin.X); pContainer.push_back(pMin.Y); pContainer.push_back(pMin.Z);
    }
};

class HexagonNodeShapeProvider:public NodeShapeProvider
{
public:
    HexagonNodeShapeProvider()
    {
        mVertexCountPerNode = 12;
    }

    void DrawNode(std::vector<float>& pContainer, const Vector3f& pNodePosition, const MazeGeometryParameters& pMazeGeometryParameters, bool pEvenRow) override
    {
        Vector3f lPosition{ pNodePosition.X, pNodePosition.Y, pNodePosition.Z -2 * DEBUG_MULT };
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

MazeDrawer::MazeDrawer(GeometryContainer& pGeometryContainer, MazeDrawerParameters pParameters) :
    mGeometryContainer(pGeometryContainer),
    mNodePositions(mGeometryContainer.GetNodePositions()),
    mMazeGeometryParameters(mGeometryContainer.GetMazeGeometryParameters()),
    mVerticesToAdd(mGeometryContainer.GetVerticesToAdd()),
    mForNodesVerticesToAdd(mGeometryContainer.GetForNodesVerticesToAdd()),
    mForNodesLut(mGeometryContainer.GetForNodesLut()),
    mForNodesCount(mGeometryContainer.GetForNodesCount()),
    mForPathVerticesToAdd(mGeometryContainer.GetForPathVerticesToAdd())
{
    SetParameters(pParameters);
}

void MazeDrawer::SetParameters(MazeDrawerParameters pParameters)
{
    mParameters = pParameters;
    delete mNodeShapeProvider;
    mNodeShapeProvider = nullptr;
    
    switch (mParameters.ShapeOfNode)
    {
    case NodeShape::Triangle:
        mNodeShapeProvider = new TriangleNodeShapeProvider();
        break;
        
    case NodeShape::Square:
        mNodeShapeProvider = new SquareNodeShapeProvider();
        break;

    case NodeShape::Hexagon:
        mNodeShapeProvider = new HexagonNodeShapeProvider();
        break;

    default :
        mNodeShapeProvider = new InvalidNodeShapeProvider();
        break;
    }

    delete mEdgeShapeProvider;
    mEdgeShapeProvider = nullptr;

    //switch (mParameters.Mode)
    //{
    //
    //}
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

    mNodePositions.resize(mMazeGeometryParameters.Width * mMazeGeometryParameters.Height, Vector3f{ FLT_MAX, FLT_MAX, FLT_MAX });

    for (size_t j = 0; j < mMazeGeometryParameters.Height; ++j)
    {
        for (size_t i = 0; i < mMazeGeometryParameters.Width; ++i)
        {
            mNodePositions[j * mMazeGeometryParameters.Width + i] = mShape->GetNodeNormalizedPosition(j * mMazeGeometryParameters.Width + i);// Vector2f{ float(i), float(j) };
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

    const Vector3f& lNodePosition = mNodePositions[pIndex];
    mForNodesLut[pIndex] = mLastLutIndex++;

    bool lIsEvenRow = ((pIndex / mMazeGeometryParameters.Width) % 2 == 0);
    mNodeShapeProvider->DrawNode(mForNodesVerticesToAdd, lNodePosition, mMazeGeometryParameters, lIsEvenRow);
}

void MazeDrawer::DrawEdge(std::vector<float>& pContainer, uint32_t pNodeIndex0, uint32_t pNodeIndex1, float pDepth)
{
    //TODO: draw edge from provider
    mShape->DrawEdge(mNodePositions, pContainer, pNodeIndex0, pNodeIndex1, pDepth, mMazeGeometryParameters.LineWidth, mParameters.ContiguousDraw);
}
