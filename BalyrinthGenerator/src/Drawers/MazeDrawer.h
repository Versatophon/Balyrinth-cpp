#pragma once

#include <Vec3.h>
#include <Labyrinth.h>

#include "GeometryContainer.h"

class MazeDrawer;
typedef void (MazeDrawer::* DrawRectangleFunc)(const Vec3& pMin, const Vec3& pMax);

class MazeDrawer:public TopologyUpdaterListener
{
public:
    MazeDrawer(GeometryContainer& pGeometryContainer);

    void AddFirstNode(uint32_t pNodeIndex) override;
    void AddEdge(uint32_t pNodeIndex0, uint32_t pNodeIndex1) override;
    void UpdaterProcessCompleted(uint32_t pPathLength, const uint32_t* pPathIndices) override;

protected:
    GeometryContainer& mGeometryContainer;
    std::vector<Vec2>& mNodePositions;
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
    Vec3i mSpaceSize = { 1, 1, 1 };
    float mHalfWidth = .5f;

    void DrawNode(uint32_t pIndex, bool pInit = false);

    void DrawAANode(const Vec3& pMin, const Vec3& pMax);
    void DrawAAEdge(const Vec3& pMin, const Vec3& pMax);
    void DrawAAPath(const Vec3& pMin, const Vec3& pMax);

    virtual void DrawEdge(DrawRectangleFunc pFunction, uint32_t pNodeIndex0, uint32_t pNodeIndex1) = 0;
};
