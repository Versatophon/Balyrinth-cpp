#pragma once

#include <Vector3f.h>
#include <Labyrinth.h>

#include "ShapeMode.h"
#include "NodeShape.h"

#include "GeometryContainer.h"

struct MazeDrawerParameters
{
    bool ContiguousDraw;
    NodeShape ShapeOfNode;
};

class NodeShapeProvider;
class EdgeShapeProvider;

class MazeDrawer;

class MazeDrawer:public TopologyUpdaterListener
{
public:
    MazeDrawer(GeometryContainer& pGeometryContainer, MazeDrawerParameters pParameters);

    void SetParameters(MazeDrawerParameters pParameters);

    size_t GetVertexCountPerNode() const;
    size_t GetVertexCountPerEdge() const;

    void AddFirstNode(uint32_t pNodeIndex) override;
    void AddEdge(uint32_t pNodeIndex0, uint32_t pNodeIndex1) override;
    void UpdaterProcessCompleted(uint32_t pPathLength, const uint32_t* pPathIndices) override;

protected:
    GeometryContainer& mGeometryContainer;
    std::vector<Vector3f>& mNodePositions;
    MazeGeometryParameters& mMazeGeometryParameters;

    std::vector<float>& mVerticesToAdd;

    std::vector<float>& mForNodesVerticesToAdd;
    std::vector<uint32_t>& mForNodesLut;
    uint32_t mLastLutIndex = 0;

    std::vector<uint8_t>& mForNodesCount;

    std::vector<float>& mForPathVerticesToAdd;

    void InitStaticVariables();

    float mLineOffset = 1.f;
    Shape* mShape = nullptr;
    Vector3f mSpaceSize = { 1, 1, 1 };
    float mHalfWidth = .5f;

    MazeDrawerParameters mParameters;

    NodeShapeProvider* mNodeShapeProvider = nullptr;
    EdgeShapeProvider* mEdgeShapeProvider = nullptr;

    void DrawNode(uint32_t pIndex, bool pInit = false);

    void DrawEdge(std::vector<float>& pContainer, uint32_t pNodeIndex0, uint32_t pNodeIndex1, float pDepth);
};
