#pragma once

#include <vector>

#include <Vector3f.h>

struct MazeGeometryParameters;
class Shape;

class GeometryContainer
{
public:
    virtual Shape* GetShape() = 0;

    virtual std::vector<Vector3f>& GetNodePositions() = 0;
    virtual MazeGeometryParameters& GetMazeGeometryParameters() = 0;
    virtual std::vector<float>& GetVerticesToAdd() = 0;

    virtual std::vector<float>& GetForNodesVerticesToAdd() = 0;
    virtual std::vector<uint32_t>& GetForNodesLut() = 0;
    virtual std::vector<uint8_t>& GetForNodesCount() = 0;

    virtual std::vector<float>& GetForPathVerticesToAdd() = 0;

    virtual void CleanupGeometry() = 0;
};
