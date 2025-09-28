#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <unordered_set>
#include <queue>

#include "ManagedWindow.h"

extern "C" {
#include <SDL3/SDL_rect.h>
}

#include <Shape.h>
#include <Labyrinth.h>
#include <Seed.h>
#include <Vector2f.h>
#include <Transformf.h>

#include <Node.h>//INVALID_NODE_INDEX

#include "Drawers/GeometryContainer.h"

class Viewport;
class ShaderProgram;
class Ubo;
class Vao;
class ArrayBuffer;

struct Matrix4f;
struct Vector2i;

template <typename T> struct SelectableGroup
{
    std::vector<std::pair<std::string, T>> mItems;
    int32_t mSelectedIndex = 0;

    inline const std::string String(int32_t pItemIndex = -1) const
    {
        return (pItemIndex >= 0 && pItemIndex < mItems.size()) ? mItems[pItemIndex].first: mItems[mSelectedIndex].first;
    }

    inline const T Item(int32_t pItemIndex = -1) const
    {
        return (pItemIndex >= 0 && pItemIndex < mItems.size()) ? mItems[pItemIndex].second : mItems[mSelectedIndex].second;
    }
};

//TODO: Use mask from black & white image to generate topology, logo, pokemon (pikachu head) 
//TODO: Use progressive generation(sort of laser engraver) to display time as number made from mazes
//TODO: ajoute feature qui montre la limite de la forme de base avec un rectangle englobant
//TODO: ajoute un moyen d'enregistrer une topologie dans un fichier

struct Color
{
    float R;
    float G;
    float B;
    float A;
};

class BalyrinthGeneratorWindow:public ManagedWindow, GeometryContainer
{
public:
    BalyrinthGeneratorWindow();
    ~BalyrinthGeneratorWindow();

    Shape* GetShape() override;
    std::vector<Vector2f>& GetNodePositions() override;
    MazeGeometryParameters& GetMazeGeometryParameters() override;
    std::vector<float>& GetVerticesToAdd() override;
    std::vector<float>& GetForNodesVerticesToAdd() override;
    std::vector<uint32_t>& GetForNodesLut() override;
    std::vector<uint8_t>& GetForNodesCount() override;
    std::vector<float>& GetForPathVerticesToAdd() override;

    void CleanupGeometry() override;

protected:
    int32_t Init() override;
    int32_t Event(SDL_Event *pEvent) override;
    int32_t Iterate() override;
    void Quit() override;
    void Render() override;
    void Resize(const Vector2i pSize) override;

private:
    bool mIsControlWindowVisible = true;

    size_t mMin = 1;
    size_t mMax = 300;

    MazeGeometryParameters mMazeGeometryParameters;

    size_t mConnectionPerFrame = 40;

    bool mRenderCells = true;
    bool mRenderEdges = true;
    bool mRenderPath = false;

    //Convert in materials
    Color mBackgroundColor { 0.2f, 0.4f, 0.2f, 1.f };
    Color* mColors = nullptr;

    Shape* mShapeProvider = nullptr;
    const Topology* mCurrentTopology = nullptr;

    LabyrinthStepper mLabyrinthStepper;
    Seed mSeed = { 1337, 4223094740 };
    bool mKeepSeed = false;

    void RegenerateLabyrinth();

    size_t mStartingPointIndex = INVALID_NODE_INDEX;

    void ProcessImGui();

    void InternalUpdateTopology();

    SelectableGroup<void*> mShapeGenerators;
    SelectableGroup<TopologyUpdaterListener*> mShapeDrawModes;
    SelectableGroup<Algorithm> mAlgorithms;

    std::unordered_set<uint32_t> mAlreadyProcessed;
    std::queue<std::pair<uint32_t, Vector2f>> mNodesToProcess;

    std::vector<Vector2f> mNodePositions;

    Viewport* mViewport = nullptr;

    ShaderProgram* mShader = nullptr;
    Ubo* mMatricesUbo = nullptr;

    Ubo* mModelsUbo = nullptr;
    Ubo* mColorsUbo = nullptr;

    bool mNeedToCleanGeometry = false;
    ArrayBuffer** mLabyrinthVBufs = nullptr;
    Vao* mLabyrinthVao = nullptr;
    uint32_t mCurrentPositionInBuffer = 0;

    ArrayBuffer** mNodesVBufs = nullptr;
    Vao* mNodesVao = nullptr;
    uint32_t mCurrentPositionInNodesBuffer = 0;
    std::vector<uint8_t> mNodesNeigborCount;


    Vector3f mRotationCenter;

    Transformf mMainTransform;
    std::vector<Transformf> mNeighborTransforms;

    // Longuest path
    ArrayBuffer** mPathVBufs = nullptr;
    Vao* mPathVao = nullptr;
    size_t mPathVertexCount = 0;

    // Cube For tests
    ArrayBuffer** mCubeVBufs = nullptr;
    Vao* mCubeVao = nullptr;

    //neighbors stuff
    bool mShowNeighbors = false;

    std::vector<float> mVerticesToUpload;
    std::vector<float> mForNodesVerticesToUpload;
    std::vector<uint32_t> mForNodesLut;

    std::vector<float> mForPathVerticesToUpload;

    Matrix4f* mMatrices = nullptr;
    Matrix4f* mModels = nullptr;

    Vector3f mEuler;
    Quaternionf mQuaternion;

    bool LoadColorConfiguration();
    void SaveColorConfiguration();
};
