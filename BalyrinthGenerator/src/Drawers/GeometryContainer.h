#pragma once

#include <vector>

#include <Vector2f.h>

class Shape;

struct Bone
{
    uint32_t NodeIndex;
    uint32_t Direction;
};

struct MazeGeometryParameters
{
    size_t Width = 10;
    size_t Height = 10;

    float PointWidth = .5f;
    float LineWidth = .25f;
};

class GeometryContainer
{
public:
    virtual Shape* GetShape() = 0;

    virtual std::vector<Vector2f>& GetNodePositions() = 0;
    virtual MazeGeometryParameters& GetMazeGeometryParameters() = 0;
    virtual std::vector<float>& GetVerticesToAdd() = 0;

    virtual std::vector<float>& GetForNodesVerticesToAdd() = 0;
    virtual std::vector<uint32_t>& GetForNodesLut() = 0;
    virtual std::vector<uint8_t>& GetForNodesCount() = 0;

    virtual std::vector<float>& GetForPathVerticesToAdd() = 0;

    virtual void CleanupGeometry() = 0;
};
