#include "MazeDrawer.h"

#include <Shape.h>

#include <cfloat>

#include <RoomNeighborhood.h>

#include "MazeGeometyParameters.h"

#define DEBUG_MULT .001f

#define SQRT3 1.7320508075688772935274463415059f
#if 0
#define SQRT3BY2 .86602540378443864676372317075294f
#define TWOBYSQRT3 1.1547005383792515290182975610039f
#endif

static Vector3f PT0{ SQRT3, 1.f, 0.f };
static Vector3f PT1{ 0.f, 2.f, 0.f };
static Vector3f PT2{ -SQRT3, 1.f, 0.f };
static Vector3f PT3{ -SQRT3, -1.f, 0.f };
static Vector3f PT4{ 0.f, -2.f, 0.f };
static Vector3f PT5{ SQRT3, -1.f, 0.f };

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
    EdgeShapeProvider(size_t pVertexCountPerEdge, float pEdgeWidth):
        mVertexCountPerEdge(pVertexCountPerEdge),
        mEdgeWidth(pEdgeWidth)
    {
    }

    inline size_t GetVertexCountPerEdge() const
    {
        return mVertexCountPerEdge;
    }

    virtual void DrawEdge(std::vector<float>& pContainer, uint32_t pDirection, const Vector3f& pNodePosition0, const Vector3f& pNodePosition1, float pDepth) = 0;

protected:
    size_t mVertexCountPerEdge;
    float mEdgeWidth = .5f;
};

//TODO::Add Grid on which you handle a topology

#define SQRT3BY2 .86602540378443864676372317075294f
#define TWOBYSQRT3 1.1547005383792515290182975610039f
#define SQRT3 1.7320508075688772935274463415059f

static Vector3f P01{ SQRT3BY2, 0.5, 0.f };
static Vector3f P12{ 0.f, 1.f, 0.f };
static Vector3f P23{ -SQRT3BY2, .5f, 0.f };
static Vector3f P34{ -SQRT3BY2, -.5f, 0.f };
static Vector3f P45{ 0.f, -1.f, 0.f };
static Vector3f P50{ SQRT3BY2, -.5f, 0.f };

#if 1
void AddAARect(std::vector<float>& pContainer, const Vector3f& pMin, const Vector3f& pMax)
{
    pContainer.push_back(pMin.X); pContainer.push_back(pMin.Y); pContainer.push_back(pMin.Z);
    pContainer.push_back(pMax.X); pContainer.push_back(pMin.Y); pContainer.push_back(pMin.Z);
    pContainer.push_back(pMax.X); pContainer.push_back(pMax.Y); pContainer.push_back(pMin.Z);
    pContainer.push_back(pMax.X); pContainer.push_back(pMax.Y); pContainer.push_back(pMin.Z);
    pContainer.push_back(pMin.X); pContainer.push_back(pMax.Y); pContainer.push_back(pMin.Z);
    pContainer.push_back(pMin.X); pContainer.push_back(pMin.Y); pContainer.push_back(pMin.Z);
}

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

        AddTriangle(pContainer, lPosition, pMazeGeometryParameters.NodeWidth / 2, pEvenRow);
    }

private:
    void AddTriangle(std::vector<float>& pContainer, const Vector3f& pCenter, const float pRadius, bool pEvenRow)
    {
        if (pEvenRow)
        {
            pContainer.push_back(pCenter.X + pRadius * PT1.X); pContainer.push_back(pCenter.Y + pRadius * PT1.Y); pContainer.push_back(pCenter.Z);
            pContainer.push_back(pCenter.X + pRadius * PT3.X); pContainer.push_back(pCenter.Y + pRadius * PT3.Y); pContainer.push_back(pCenter.Z);
            pContainer.push_back(pCenter.X + pRadius * PT5.X); pContainer.push_back(pCenter.Y + pRadius * PT5.Y); pContainer.push_back(pCenter.Z);
        }
        else
        {
            pContainer.push_back(pCenter.X + pRadius * PT0.X); pContainer.push_back(pCenter.Y + pRadius * PT0.Y); pContainer.push_back(pCenter.Z);
            pContainer.push_back(pCenter.X + pRadius * PT2.X); pContainer.push_back(pCenter.Y + pRadius * PT2.Y); pContainer.push_back(pCenter.Z);
            pContainer.push_back(pCenter.X + pRadius * PT4.X); pContainer.push_back(pCenter.Y + pRadius * PT4.Y); pContainer.push_back(pCenter.Z);
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
        Vector3f lMin{ pNodePosition.X - pMazeGeometryParameters.NodeWidth / 2, pNodePosition.Y - pMazeGeometryParameters.NodeWidth / 2, pNodePosition.Z -2 * DEBUG_MULT };
        Vector3f lMax{ pNodePosition.X + pMazeGeometryParameters.NodeWidth / 2, pNodePosition.Y + pMazeGeometryParameters.NodeWidth / 2, pNodePosition.Z -2 * DEBUG_MULT };
        AddAARect(pContainer, lMin, lMax);
    }

private:
    
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
        AddHexagon(pContainer, lPosition, pMazeGeometryParameters.NodeWidth);
    }

private:
    void AddHexagon(std::vector<float>& pContainer, const Vector3f& pCenter, const float pRadius)
    {
        pContainer.push_back(pCenter.X + pRadius * P50.X); pContainer.push_back(pCenter.Y + pRadius * P50.Y); pContainer.push_back(pCenter.Z);
        pContainer.push_back(pCenter.X + pRadius * P01.X); pContainer.push_back(pCenter.Y + pRadius * P01.Y); pContainer.push_back(pCenter.Z);
        pContainer.push_back(pCenter.X + pRadius * P12.X); pContainer.push_back(pCenter.Y + pRadius * P12.Y); pContainer.push_back(pCenter.Z);

        pContainer.push_back(pCenter.X + pRadius * P12.X); pContainer.push_back(pCenter.Y + pRadius * P12.Y); pContainer.push_back(pCenter.Z);
        pContainer.push_back(pCenter.X + pRadius * P23.X); pContainer.push_back(pCenter.Y + pRadius * P23.Y); pContainer.push_back(pCenter.Z);
        pContainer.push_back(pCenter.X + pRadius * P50.X); pContainer.push_back(pCenter.Y + pRadius * P50.Y); pContainer.push_back(pCenter.Z);

        pContainer.push_back(pCenter.X + pRadius * P45.X); pContainer.push_back(pCenter.Y + pRadius * P45.Y); pContainer.push_back(pCenter.Z);
        pContainer.push_back(pCenter.X + pRadius * P50.X); pContainer.push_back(pCenter.Y + pRadius * P50.Y); pContainer.push_back(pCenter.Z);
        pContainer.push_back(pCenter.X + pRadius * P23.X); pContainer.push_back(pCenter.Y + pRadius * P23.Y); pContainer.push_back(pCenter.Z);

        pContainer.push_back(pCenter.X + pRadius * P23.X); pContainer.push_back(pCenter.Y + pRadius * P23.Y); pContainer.push_back(pCenter.Z);
        pContainer.push_back(pCenter.X + pRadius * P34.X); pContainer.push_back(pCenter.Y + pRadius * P34.Y); pContainer.push_back(pCenter.Z);
        pContainer.push_back(pCenter.X + pRadius * P45.X); pContainer.push_back(pCenter.Y + pRadius * P45.Y); pContainer.push_back(pCenter.Z);
    }
};
#endif

#if 1
class SquareEdgeShapeProvider:public EdgeShapeProvider
{
public:
    SquareEdgeShapeProvider(float pEdgeWidth) :EdgeShapeProvider(12, pEdgeWidth)
    {
    }

    void DrawEdge(std::vector<float>& pContainer, uint32_t pDirection, const Vector3f& pNodePosition0, const Vector3f& pNodePosition1, float pDepth) override
    {
        float lLineOffset = mEdgeWidth / 2.f;

        Vector3f lNode0Min;
        Vector3f lNode0Max;
        Vector3f lNode1Min;
        Vector3f lNode1Max;

        switch (pDirection)
        {
        case 0:
            lNode0Min = { pNodePosition0.X - lLineOffset, pNodePosition0.Y - lLineOffset, pDepth };
            lNode0Max = { pNodePosition0.X + .5f, pNodePosition0.Y + lLineOffset, pDepth };
            lNode1Min = { pNodePosition1.X - .5f, pNodePosition1.Y - lLineOffset, pDepth };
            lNode1Max = { pNodePosition1.X + lLineOffset, pNodePosition1.Y + lLineOffset, pDepth };
            break;

        case 1:
            lNode0Min = { pNodePosition0.X - lLineOffset, pNodePosition0.Y - lLineOffset, pDepth };
            lNode0Max = { pNodePosition0.X + lLineOffset, pNodePosition0.Y + .5f, pDepth };
            lNode1Min = { pNodePosition1.X - lLineOffset, pNodePosition1.Y - .5f, pDepth };
            lNode1Max = { pNodePosition1.X + lLineOffset, pNodePosition1.Y + lLineOffset, pDepth };
            break;

        case 2:
            lNode0Min = { pNodePosition0.X - 0.5f, pNodePosition0.Y - lLineOffset, pDepth };
            lNode0Max = { pNodePosition0.X + lLineOffset, pNodePosition0.Y + lLineOffset, pDepth };
            lNode1Min = { pNodePosition1.X - lLineOffset, pNodePosition1.Y - lLineOffset, pDepth };
            lNode1Max = { pNodePosition1.X + 0.5f, pNodePosition1.Y + lLineOffset, pDepth };
            break;

        case 3:
            lNode0Min = { pNodePosition0.X - lLineOffset, pNodePosition0.Y - .5f, pDepth };
            lNode0Max = { pNodePosition0.X + lLineOffset, pNodePosition0.Y + lLineOffset, pDepth };
            lNode1Min = { pNodePosition1.X - lLineOffset, pNodePosition1.Y - lLineOffset, pDepth };
            lNode1Max = { pNodePosition1.X + lLineOffset, pNodePosition1.Y + .5f, pDepth };
            break;
        }

        AddAARect(pContainer, lNode0Min, lNode0Max);
        AddAARect(pContainer, lNode1Min, lNode1Max);
    }
};

class HexagonEdgeShapeProvider:public EdgeShapeProvider
{
public:
    HexagonEdgeShapeProvider(float pEdgeWidth):EdgeShapeProvider(18, pEdgeWidth),
        mP0((pEdgeWidth / 3.f)* (P50 + P01)),
        mP1((pEdgeWidth / 3.f)* (P01 + P12)),
        mP2((pEdgeWidth / 3.f)* (P12 + P23)),
        mP3((pEdgeWidth / 3.f)* (P23 + P34)),
        mP4((pEdgeWidth / 3.f)* (P34 + P45)),
        mP5((pEdgeWidth / 3.f)* (P45 + P50))
    {
        //     mP2 mP1
        // mP3         mP0
        //     mP4 mP5

        mMid0_0 = (mP4 + mP5 + Vector3f{ SQRT3, 0, 0 }) / 2.f;
        mMid0_1 = (mP2 + mP1 + Vector3f{ SQRT3, 0, 0 }) / 2.f;

        mMid1_0 = (mP5 + mP0 + Vector3f{ SQRT3BY2, 1.5f, 0 }) / 2.f;
        mMid1_1 = (mP3 + mP2 + Vector3f{ SQRT3BY2, 1.5f, 0 }) / 2.f;

        mMid2_0 = (mP0 + mP1 + Vector3f{ -SQRT3BY2, 1.5f, 0 }) / 2.f;
        mMid2_1 = (mP4 + mP3 + Vector3f{ -SQRT3BY2, 1.5f, 0 }) / 2.f;

        mMid3_0 = (mP1 + mP2 + Vector3f{ -SQRT3, 0, 0 }) / 2.f;
        mMid3_1 = (mP5 + mP4 + Vector3f{ -SQRT3, 0, 0 }) / 2.f;

        mMid4_0 = (mP2 + mP3 + Vector3f{ -SQRT3BY2, -1.5f, 0 }) / 2.f;
        mMid4_1 = (mP0 + mP5 + Vector3f{ -SQRT3BY2, -1.5f, 0 }) / 2.f;

        mMid5_0 = (mP3 + mP4 + Vector3f{ SQRT3BY2, -1.5f, 0 }) / 2.f;
        mMid5_1 = (mP1 + mP0 + Vector3f{ SQRT3BY2, -1.5f, 0 }) / 2.f;
    }

    void DrawEdge(std::vector<float>& pContainer, uint32_t pDirection, const Vector3f& pNodePosition0, const Vector3f& pNodePosition1, float pDepth) override
    {
        Vector3f lP0_0;
        Vector3f lP0_1;
        Vector3f lP0_2;
        Vector3f lP0_3;
        Vector3f lP0_4;

        Vector3f lP1_0;
        Vector3f lP1_1;
        Vector3f lP1_2;
        Vector3f lP1_3;
        Vector3f lP1_4;

        //       lP0_3 lP0_2  lP1_4 lP1_3
        //lP0_4                            lP1_2
        //       lP0_0 lP0_1  lP1_0 lP1_1

        switch (pDirection)
        {
        case 0:
            lP0_0 = pNodePosition0 + mP4;
            lP0_1 = pNodePosition0 + mMid0_0;
            lP0_2 = pNodePosition0 + mMid0_1;
            lP0_3 = pNodePosition0 + mP2;
            lP0_4 = pNodePosition0 + mP3;

            lP1_0 = pNodePosition1 + mMid3_1;
            lP1_1 = pNodePosition1 + mP5;
            lP1_2 = pNodePosition1 + mP0;
            lP1_3 = pNodePosition1 + mP1;
            lP1_4 = pNodePosition1 + mMid3_0;
            break;
        case 1:
            lP0_0 = pNodePosition0 + mP5;
            lP0_1 = pNodePosition0 + mMid1_0;
            lP0_2 = pNodePosition0 + mMid1_1;
            lP0_3 = pNodePosition0 + mP3;
            lP0_4 = pNodePosition0 + mP4;

            lP1_0 = pNodePosition1 + mMid4_1;
            lP1_1 = pNodePosition1 + mP0;
            lP1_2 = pNodePosition1 + mP1;
            lP1_3 = pNodePosition1 + mP2;
            lP1_4 = pNodePosition1 + mMid4_0;
            break;
        case 2:
            lP0_0 = pNodePosition0 + mP0;
            lP0_1 = pNodePosition0 + mMid2_0;
            lP0_2 = pNodePosition0 + mMid2_1;
            lP0_3 = pNodePosition0 + mP4;
            lP0_4 = pNodePosition0 + mP5;

            lP1_0 = pNodePosition1 + mMid5_1;
            lP1_1 = pNodePosition1 + mP1;
            lP1_2 = pNodePosition1 + mP2;
            lP1_3 = pNodePosition1 + mP3;
            lP1_4 = pNodePosition1 + mMid5_0;
            break;
        case 3:
            lP0_0 = pNodePosition0 + mP1;
            lP0_1 = pNodePosition0 + mMid3_0;
            lP0_2 = pNodePosition0 + mMid3_1;
            lP0_3 = pNodePosition0 + mP5;
            lP0_4 = pNodePosition0 + mP0;

            lP1_0 = pNodePosition1 + mMid0_1;
            lP1_1 = pNodePosition1 + mP2;
            lP1_2 = pNodePosition1 + mP3;
            lP1_3 = pNodePosition1 + mP4;
            lP1_4 = pNodePosition1 + mMid0_0;
            break;
        case 4:
            lP0_0 = pNodePosition0 + mP2;
            lP0_1 = pNodePosition0 + mMid4_0;
            lP0_2 = pNodePosition0 + mMid4_1;
            lP0_3 = pNodePosition0 + mP0;
            lP0_4 = pNodePosition0 + mP1;

            lP1_0 = pNodePosition1 + mMid1_1;
            lP1_1 = pNodePosition1 + mP3;
            lP1_2 = pNodePosition1 + mP4;
            lP1_3 = pNodePosition1 + mP5;
            lP1_4 = pNodePosition1 + mMid1_0;
            break;
        case 5:
            lP0_0 = pNodePosition0 + mP3;
            lP0_1 = pNodePosition0 + mMid5_0;
            lP0_2 = pNodePosition0 + mMid5_1;
            lP0_3 = pNodePosition0 + mP1;
            lP0_4 = pNodePosition0 + mP2;

            lP1_0 = pNodePosition1 + mMid2_1;
            lP1_1 = pNodePosition1 + mP4;
            lP1_2 = pNodePosition1 + mP5;
            lP1_3 = pNodePosition1 + mP0;
            lP1_4 = pNodePosition1 + mMid2_0;
            break;
        }

        pContainer.push_back(lP0_3.X); pContainer.push_back(lP0_3.Y); pContainer.push_back(pDepth);
        pContainer.push_back(lP0_4.X); pContainer.push_back(lP0_4.Y); pContainer.push_back(pDepth);
        pContainer.push_back(lP0_0.X); pContainer.push_back(lP0_0.Y); pContainer.push_back(pDepth);

        pContainer.push_back(lP0_0.X); pContainer.push_back(lP0_0.Y); pContainer.push_back(pDepth);
        pContainer.push_back(lP0_1.X); pContainer.push_back(lP0_1.Y); pContainer.push_back(pDepth);
        pContainer.push_back(lP0_2.X); pContainer.push_back(lP0_2.Y); pContainer.push_back(pDepth);

        pContainer.push_back(lP0_2.X); pContainer.push_back(lP0_2.Y); pContainer.push_back(pDepth);
        pContainer.push_back(lP0_3.X); pContainer.push_back(lP0_3.Y); pContainer.push_back(pDepth);
        pContainer.push_back(lP0_0.X); pContainer.push_back(lP0_0.Y); pContainer.push_back(pDepth);

        pContainer.push_back(lP1_3.X); pContainer.push_back(lP1_3.Y); pContainer.push_back(pDepth);
        pContainer.push_back(lP1_4.X); pContainer.push_back(lP1_4.Y); pContainer.push_back(pDepth);
        pContainer.push_back(lP1_0.X); pContainer.push_back(lP1_0.Y); pContainer.push_back(pDepth);

        pContainer.push_back(lP1_0.X); pContainer.push_back(lP1_0.Y); pContainer.push_back(pDepth);
        pContainer.push_back(lP1_1.X); pContainer.push_back(lP1_1.Y); pContainer.push_back(pDepth);
        pContainer.push_back(lP1_3.X); pContainer.push_back(lP1_3.Y); pContainer.push_back(pDepth);

        pContainer.push_back(lP1_3.X); pContainer.push_back(lP1_3.Y); pContainer.push_back(pDepth);
        pContainer.push_back(lP1_1.X); pContainer.push_back(lP1_1.Y); pContainer.push_back(pDepth);
        pContainer.push_back(lP1_2.X); pContainer.push_back(lP1_2.Y); pContainer.push_back(pDepth);
    }

private:
    Vector3f mP0;
    Vector3f mP1;
    Vector3f mP2;
    Vector3f mP3;
    Vector3f mP4;
    Vector3f mP5;

    Vector3f mMid0_0;
    Vector3f mMid0_1;

    Vector3f mMid1_0;
    Vector3f mMid1_1;

    Vector3f mMid2_0;
    Vector3f mMid2_1;

    Vector3f mMid3_0;
    Vector3f mMid3_1;

    Vector3f mMid4_0;
    Vector3f mMid4_1;

    Vector3f mMid5_0;
    Vector3f mMid5_1;

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

    default:
        mNodeShapeProvider = new InvalidNodeShapeProvider();
        break;
    }
}

size_t MazeDrawer::GetVertexCountPerNode() const
{
    return mNodeShapeProvider->GetVertexCountPerNode();
}

size_t MazeDrawer::GetVertexCountPerEdge() const
{
    return mEdgeShapeProvider->GetVertexCountPerEdge();
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
            mNodePositions[j * mMazeGeometryParameters.Width + i] = mShape->GetNodeNormalizedPosition(j * mMazeGeometryParameters.Width + i);
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
    mLineOffset = mMazeGeometryParameters.EdgeWidth / 2.f;
    mShape = mGeometryContainer.GetShape();
    mSpaceSize = mShape->GetSpaceSize();
    mHalfWidth = (1.f / 2.f) + mLineOffset;

    //HACK: not good place
    delete mEdgeShapeProvider;
    mEdgeShapeProvider = nullptr;

    if (mShape != nullptr)
    {
        switch (mShape->GetGridShape())
        {
        case GridShape::Square:
            mEdgeShapeProvider = new SquareEdgeShapeProvider(mMazeGeometryParameters.EdgeWidth);
            break;

        case GridShape::Hexagon:
            mEdgeShapeProvider = new HexagonEdgeShapeProvider(mMazeGeometryParameters.EdgeWidth);
            break;

        default:
            break;
        }
    }
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
    const Vector3f& lNode0Position = mNodePositions[pNodeIndex0];
    Vector3f& lNode1Position = mNodePositions[pNodeIndex1];
    uint32_t lDirection = mShape->GetRoomNeighborhood()->GetDirection(pNodeIndex0, pNodeIndex1);

    if (mParameters.ContiguousDraw)
    {
        Vector3f lSpaceSize = mShape->GetSpaceSize();
        
        Vector3f lSpaceDelta = mShape->GetUnitSpaceDelta(pNodeIndex0, pNodeIndex1);

        Vector3f lNodeNormalizedPos0 = mShape->GetNodeNormalizedPosition(pNodeIndex0);
        Vector3f lNodeNormalizedPos1 = mShape->GetNodeNormalizedPosition(pNodeIndex1);

        Vector3f lInitialNode0Position = { lNodeNormalizedPos0.X, lNodeNormalizedPos0.Y, 0 };

        //Since the space delta is from node 0 to node 1, we need to compensate it with opposited delta
        Vector3f lInitialNode1Position = { (lNodeNormalizedPos1.X - lSpaceDelta.X * lSpaceSize.X),
                                           (lNodeNormalizedPos1.Y - lSpaceDelta.Y * lSpaceSize.Y), 0 };

        lNode1Position = lNode0Position + (lInitialNode1Position - lInitialNode0Position);
    }
    
    //TODO: draw edge from provider
    mEdgeShapeProvider->DrawEdge(pContainer, lDirection, lNode0Position, lNode1Position, pDepth);
}
