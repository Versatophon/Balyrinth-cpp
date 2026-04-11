#include "BalyrinthGeneratorWindow.h"

#include <GL/glew.h>

extern "C" {
#include <SDL3/SDL.h>
}

#include <queue>
#include <iostream>
#include <random>

#include <imgui.h>
#include <imgui_internal.h>

#include <Topology.h>
#include <Labyrinth.h>

#include <Matrix4f.h>
#include <Vector2i.h>
#include <Trigonometry.h>

#include "GL/RenderableMesh.h"
#include "GL/Shader.h"
#include "GL/ShaderProgram.h"
#include "GL/Viewport.h"

#include "GL/Buffers/ArrayBuffer.h"
#include "GL/Buffers/Ubo.h"

#include "Resources/InlineShaders.h"

//#include "Drawers/ContiguousMazeDrawer.h"
//#include "Drawers/ShapeMazeDrawer.h"
#include "Drawers/MazeDrawer.h"

#include "Tools/SettingsLoadSave.h"

#include "GL/Renderable.h"
#include "GL/Binder.h"

//TODO: utiliser une table d'indirection contenant les coordonnées normalisées de chaque node, avec des générateurs
//TODO: ajouter une fonctionnalité pour sérialiser les topologies
//TODO: ajouter une fonctionnalité pour exporter la seed

template <typename T> bool ExecuteCombobox(const char* pLabel, SelectableGroup<T>& pSelectableGroup)
{
    bool lChanged = false;

    if (ImGui::BeginCombo(pLabel, pSelectableGroup.String().c_str()))
    {
        for (int32_t i = 0; i < pSelectableGroup.mItems.size(); ++i)
        {
            bool lCurrentItemSelected = (pSelectableGroup.mSelectedIndex == i);

            if (ImGui::Selectable(pSelectableGroup.String(i).c_str(), lCurrentItemSelected))
            {
                pSelectableGroup.mSelectedIndex = i;
                lChanged = true;
            }
        }
        ImGui::EndCombo();
    }

    return lChanged;
}

BalyrinthGeneratorWindow::BalyrinthGeneratorWindow(): ManagedWindow(0, nullptr),
mLabyrinthStepper(LabyrinthStepper({ RoomSelect::Last, Backtrack::Queue, ComputeDirection::Any })),
    mViewport(new Viewport)
{
    mShapeGenerators = {{{"Squares On Tore", (void*)GenerateSquaresOnToreShape},
                         {"Squares On Rect", (void*)GenerateSquaresOnRectShape},
                         {"Hexagons On Tore", (void*)GenerateHexagonsOnToreShape},
                         {"Hexagons On Rect", (void*)GenerateHexagonsOnRectShape}}};

    mShapeGenerators.mSelectedIndex = 0;

    mNodeShapeModes = {{{"Triangle", NodeShape::Triangle},
                        {"Square", NodeShape::Square},
                        {"Pentagon", NodeShape::Pentagon},
                        {"Hexagon", NodeShape::Hexagon},
                        {"Heptagon", NodeShape::Heptagon},
                        {"Octogon", NodeShape::Octogon}}};

    mNodeShapeModes.mSelectedIndex = 1;

    mRoomSelectMode = {{{"Last Room Added", RoomSelect::Last},
                        {"Fill Room", RoomSelect::Fill}} };

    mBacktrackModes = {{{"BT Stack", Backtrack::Stack},
                        {"BT Queue", Backtrack::Queue},
                        {"BT Random", Backtrack::Random}}};

    mComputeDirectionModes = {{{"Any", ComputeDirection::Any},
                               {"Force Change", ComputeDirection::ForceChange}}};

    mMazeDrawer = new MazeDrawer(*this, {/*mShapeModes.Item()*/mContiguousDraw, mNodeShapeModes.Item()});

    mLabyrinthStepper.SetUpdateListener(mMazeDrawer);

    {
        mForNodesLut.resize(mMazeGeometryParameters.Height * mMazeGeometryParameters.Width);
        mNodesNeighborCount.resize(mMazeGeometryParameters.Height * mMazeGeometryParameters.Width * mMazeDrawer->GetVertexCountPerNode());

        memset(mNodesNeighborCount.data(), 0, mNodesNeighborCount.size());
    }

    InternalUpdateTopology();
    mLabyrinthStepper.InitiateGeneration(&mSeed);

    mNeighborTransforms.resize(8);

    mMainTransform.Position.Z = -300;
}
 
BalyrinthGeneratorWindow::~BalyrinthGeneratorWindow()
{
    //for (std::pair<std::string, TopologyUpdaterListener*> lListener : mShapeDrawModes.mItems)
    //{
    //    delete lListener.second;
    //}

    delete mMazeDrawer;

    delete mViewport;
}

Shape* BalyrinthGeneratorWindow::GetShape()
{
    return mShapeProvider;
}

std::vector<Vector3f>& BalyrinthGeneratorWindow::GetNodePositions()
{
    return mNodePositions;
}

MazeGeometryParameters& BalyrinthGeneratorWindow::GetMazeGeometryParameters()
{
    return mMazeGeometryParameters;
}

std::vector<float>& BalyrinthGeneratorWindow::GetVerticesToAdd()
{
    return mVerticesToUpload;
}

std::vector<float>& BalyrinthGeneratorWindow::GetForNodesVerticesToAdd()
{
    return mForNodesVerticesToUpload;
}

std::vector<uint32_t>& BalyrinthGeneratorWindow::GetForNodesLut()
{
    return mForNodesLut;
}

std::vector<uint8_t>& BalyrinthGeneratorWindow::GetForNodesCount()
{
    return mNodesNeighborCount;
}

std::vector<float>& BalyrinthGeneratorWindow::GetForPathVerticesToAdd()
{
    return mForPathVerticesToUpload;
}

void BalyrinthGeneratorWindow::CleanupGeometry()
{
    mNeedToCleanGeometry = true;

    mVerticesToUpload.clear();
    mForNodesVerticesToUpload.clear();
    mForPathVerticesToUpload.clear();

    mForNodesLut.resize(mMazeGeometryParameters.Height * mMazeGeometryParameters.Width);
    mNodesNeighborCount.resize(mMazeGeometryParameters.Height * mMazeGeometryParameters.Width * mMazeDrawer->GetVertexCountPerNode());

    memset(mNodesNeighborCount.data(), 0, mNodesNeighborCount.size() * sizeof(uint8_t));
}

int32_t BalyrinthGeneratorWindow::Init()
{
    GLenum lReturn = glewInit();

    mGraphicsState.Init();

    mMatricesUbo = new Ubo(sizeof(Matrix4f) * 2, "matrices");
    mMatrices = (Matrix4f*)mMatricesUbo->GetMemory();

    mModelsUbo = new Ubo(sizeof(Matrix4f) * 256, "models");
    mModels = (Matrix4f*)mModelsUbo->GetMemory();

    mColorsUbo = new Ubo(sizeof(Color) * 256, "colors");
    mColors = (Color*)mColorsUbo->GetMemory();

    if (!LoadColorConfiguration())
    {//if conf read fails
        mColors[0] = { 0.f, 1.f, 0.f, 1.f };
        mColors[1] = { 0, float(0x55) / float(0xFF), 0, float(0xFF) / float(0xFF) };
        mColors[2] = { 0, float(0xAA) / float(0xFF), 0, float(0xFF) / float(0xFF) };
        mColors[3] = { 0, float(0xFF) / float(0xFF), 0, float(0xFF) / float(0xFF) };
        mColors[4] = { float(0xFF) / float(0xFF), 0, 0, float(0xFF) / float(0xFF) };
        mColors[5] = { float(0xFF) / float(0xFF), 0, 0, float(0xFF) / float(0xFF) };
        mColors[6] = { float(0xFF) / float(0xFF), 0, 0, float(0xFF) / float(0xFF) };
        mColors[7] = { float(0xFF) / float(0xFF), 0, 0, float(0xFF) / float(0xFF) };
        mColors[8] = { float(0xFF) / float(0xFF), 0, 0, float(0xFF) / float(0xFF) };
        mColors[9] = { float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF) };
        
        mColors[10] = { float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF) };
        mColors[11] = { float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF) };
        mColors[12] = { float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF) };
        mColors[13] = { float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF) };
        mColors[14] = { float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF) };
        mColors[15] = { float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF) };
        mColors[16] = { float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF) };
        mColors[17] = { float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF) };
    }

    mMatrices[0] = Matrix4f::Id;
    mMatrices[1] = Matrix4f::Id;

    mMatrices[1].SetPosition({ 0, 0, -10000 });

    mModels[0] = Matrix4f::Id;

    mLabyrinthShader = new ShaderProgram;
    mNodeShader = new ShaderProgram;
    mPathShader = new ShaderProgram;

    Shader* lVertexShader = new Shader(ShaderType::VertexShader);
    Shader* lVertexShaderSingleColor = new Shader(ShaderType::VertexShader);
    Shader* lFragmentShader = new Shader(ShaderType::FragmentShader);

    lVertexShader->LoadFromString(sViewVertexShSource);
    lVertexShaderSingleColor->LoadFromString(sViewVertexSingleColorShSource);
    lFragmentShader->LoadFromString(sViewFragmentShSource);

    {
        mNodeShader->AttachShader(lVertexShader);
        mNodeShader->AttachShader(lFragmentShader);

        mNodeShader->Link();

        mNodeShader->AddAttribute("vPos", AttributeType::Float);
        mNodeShader->AddAttribute("vColIndex", AttributeType::Integer);
        mNodeShader->AddUniform("model_index");

        {
            Binder lBinder(*mNodeShader);
            mNodeShader->UpdateUniform("model_index", 0);
        }

        mNodeShader->LinkUbo(mMatricesUbo);
        mNodeShader->LinkUbo(mModelsUbo);
        mNodeShader->LinkUbo(mColorsUbo);
    }

    {
        mLabyrinthShader->AttachShader(lVertexShaderSingleColor);
        mLabyrinthShader->AttachShader(lFragmentShader);

        mLabyrinthShader->Link();

        mLabyrinthShader->AddAttribute("vPos", AttributeType::Float);
        mLabyrinthShader->AddUniform("model_index");
        mLabyrinthShader->AddUniform("color_index");

        {
            Binder lBinder(*mLabyrinthShader);
            mLabyrinthShader->UpdateUniform("model_index", 0);
            mLabyrinthShader->UpdateUniform("color_index", 0);
        }

        mLabyrinthShader->LinkUbo(mMatricesUbo);
        mLabyrinthShader->LinkUbo(mModelsUbo);
        mLabyrinthShader->LinkUbo(mColorsUbo);
    }

    {
        mPathShader->AttachShader(lVertexShaderSingleColor);
        mPathShader->AttachShader(lFragmentShader);

        mPathShader->Link();

        mPathShader->AddAttribute("vPos", AttributeType::Float);
        mPathShader->AddUniform("model_index");
        mPathShader->AddUniform("color_index");

        {
            Binder lBinder(*mPathShader);
            mPathShader->UpdateUniform("model_index", 0);
            mPathShader->UpdateUniform("color_index", 9);
        }

        mPathShader->LinkUbo(mMatricesUbo);
        mPathShader->LinkUbo(mModelsUbo);
        mPathShader->LinkUbo(mColorsUbo);
    }

    uint32_t lVertexCount = (((mMazeGeometryParameters.Height * mMazeGeometryParameters.Width) - 1) + (mMazeGeometryParameters.Height + mMazeGeometryParameters.Width) * 2) * 12;//HACK

    //size_t lItemSizes[] = {sizeof(float) * 3, sizeof(uint32_t) * 1};
    size_t lItemSizes[] = {sizeof(float) * 3, sizeof(uint8_t) * 1};

    //Initialize edges geometry
    mRenderableLabyrinth = new Renderable(mLabyrinthShader, 1, lItemSizes, lVertexCount);

    //Initialize nodes geometry
    mRenderableNodes = new Renderable(mNodeShader, 2, lItemSizes, mMazeGeometryParameters.Height * mMazeGeometryParameters.Width * mMazeDrawer->GetVertexCountPerNode());

    mRenderableLongestPath = new Renderable(mPathShader, 1, lItemSizes, 0);

    {
        uint8_t lBaseIndex = 10;

        std::vector<Vector3f> lCollapsedVertices =
        {
            {-1.f, -1.f, -1.f},//0
            {1.f, -1.f, -1.f},//1

            {-1.f, 1.f, -1.f},//2
            {1.f, 1.f, -1.f},//3

            {-1.f, -1.f, 1.f},//4
            {1.f, -1.f, 1.f},//5

            {-1.f, 1.f, 1.f},//6
            {1.f, 1.f, 1.f},//7
        };

        std::vector<uint16_t> lEdgeIndices =
        {
            0, 1, 2, 3,
            0, 2, 1, 3,
            4, 5, 6, 7,
            4, 6, 5, 7,
            0, 4, 1, 5,
            3, 7, 2, 6,
        };

        std::vector<uint8_t> lCollapsedColIndices =
        {
            uint8_t(lBaseIndex + 0),
            uint8_t(lBaseIndex + 1),
            uint8_t(lBaseIndex + 2),
            uint8_t(lBaseIndex + 3),
            uint8_t(lBaseIndex + 4),
            uint8_t(lBaseIndex + 5),
            uint8_t(lBaseIndex + 6),
            uint8_t(lBaseIndex + 7),
        };

        mCubeMesh = new RenderableMesh(mNodeShader, lCollapsedVertices, lCollapsedColIndices, lEdgeIndices, GeometryType::Lines);
    }

    delete lVertexShader;
    delete lFragmentShader;

    mMainTransform.SetScale(100.f);

    mRenderableLabyrinth->SetItemCount(0);
    mVerticesToUpload.clear();

    Resize(Vector2i{ (int32_t)GetWidth(), (int32_t)GetHeight() });

    return SDL_APP_CONTINUE;
}

int32_t BalyrinthGeneratorWindow::Event(SDL_Event *pEvent)
{
    switch ( pEvent->type )
    {
    case SDL_EVENT_KEY_DOWN:

        switch (pEvent->key.key)
        {
        case SDLK_S://S for "Show"
            mIsControlWindowVisible = true;
            break;

        case SDLK_SPACE:
            RegenerateLabyrinth();
            break;

        case SDLK_C:
            //TODO: center and scle to fit screen here
            break;
        default:
            break;
        }
        break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (pEvent->button.button == 3)
        {
            mRotationCenter.X = pEvent->button.x;
            mRotationCenter.Y = GetHeight() - pEvent->button.y;
        }
        break;
    case SDL_EVENT_MOUSE_MOTION:
        if (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_LMASK)
        {
            Transformf lNullTransform;
            mMainTransform.Translate({ pEvent->motion.xrel, -pEvent->motion.yrel, 0 }, &lNullTransform);
        }
        if (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_RMASK)
        {
            Transformf lMouseTransform
            {
                mRotationCenter,
                {0.f, 0.f, 0.f, 1.f},
                1.f
            };
            Transformf lNullTransform;
            mMainTransform.Rotate(Quaternionf(Vector3f::ZUnit, -pEvent->motion.xrel*0.01f), &lNullTransform, &lMouseTransform);
        }
        break;
    case SDL_EVENT_MOUSE_WHEEL:
    {
        float lZoomFactor = 1.2f;
        Transformf lMouseTransform
        {
            {pEvent->wheel.mouse_x, GetHeight() - pEvent->wheel.mouse_y, 0},
            {1.f, 0.f, 0.f ,0.f},
            1.f
        };
        mMainTransform.Rescale(pow(lZoomFactor, pEvent->wheel.y), &lMouseTransform);

    }
        
        break;
    default:
        break;

    }

    return SDL_APP_CONTINUE;
}

int32_t BalyrinthGeneratorWindow::Iterate()
{
    if (mConnectionPerSecond != 0.)
    {
        int32_t lConnectionCountToProcess = mConnectionPerSecond * mElapsedTime;
        mElapsedTime += GetLastFrameDuration();
        mElapsedTime -= (lConnectionCountToProcess / mConnectionPerSecond);
        mLabyrinthStepper.ProcessStep(lConnectionCountToProcess, 0.01f);
    }

    ProcessImGui();

    return SDL_APP_CONTINUE;
}

void BalyrinthGeneratorWindow::Quit()
{
    SaveColorConfiguration();

    delete mLabyrinthShader;
    delete mNodeShader;
    delete mPathShader;

    delete mMatricesUbo;
    delete mModelsUbo;
    delete mColorsUbo;
}

void BalyrinthGeneratorWindow::Render()
{
    if (mNeedToCleanGeometry)
    {
        uint32_t lMaxVertexCount = ((mMazeGeometryParameters.Height * mMazeGeometryParameters.Width) - 1) * mMazeDrawer->GetVertexCountPerEdge();

        //TODO: use a different shader instead of uploading a color index array
        mRenderableLabyrinth->SetItemCount(lMaxVertexCount);

        mRenderableNodes->SetItemCount(mMazeGeometryParameters.Height * mMazeGeometryParameters.Width * mMazeDrawer->GetVertexCountPerNode());

        mRenderableLongestPath->SetItemCount(0);
        mNeedToCleanGeometry = false;
    }

    if (!mVerticesToUpload.empty())
    {
        size_t lVertexCountToUpload = mVerticesToUpload.size() / 3;

        mRenderableLabyrinth->Append(0, lVertexCountToUpload, mVerticesToUpload.data());

        mVerticesToUpload.clear();
    }

    if (!mForNodesVerticesToUpload.empty())
    {
        size_t lVertexCountToUpload = mForNodesVerticesToUpload.size() / 3;

        mRenderableNodes->Append(0, lVertexCountToUpload, mForNodesVerticesToUpload.data());
        mRenderableNodes->Update(1, 0, mRenderableNodes->GetMaxVertexCount(), mNodesNeighborCount.data());

        mForNodesVerticesToUpload.clear();
    }

    if (!mForPathVerticesToUpload.empty())
    {
        size_t lVertexCountToUpload = mForPathVerticesToUpload.size() / 3;
        mRenderableLongestPath->SetItemCount(lVertexCountToUpload);
        mRenderableLongestPath->Append(0, lVertexCountToUpload, mForPathVerticesToUpload.data());

        mForPathVerticesToUpload.clear();
    }

    {
        Vector3f lSpaceSize = mShapeProvider->GetSpaceSize();
        float lHOffset = lSpaceSize.X;
        float lVOffset = lSpaceSize.Y;

        Transformf lTransform = mMainTransform;

        lTransform.Translate({ lHOffset, 0, 0 });
        mModels[1] = lTransform.GetMatrix();

        lTransform.Translate({ 0, -lVOffset, 0 });
        mModels[2] = lTransform.GetMatrix();

        lTransform.Translate({ -lHOffset, 0, 0 });
        mModels[3] = lTransform.GetMatrix();

        lTransform.Translate({ -lHOffset, 0, 0 });
        mModels[4] = lTransform.GetMatrix();

        lTransform.Translate({ 0, lVOffset, 0 });
        mModels[5] = lTransform.GetMatrix();

        lTransform.Translate({ 0, lVOffset, 0 });
        mModels[6] = lTransform.GetMatrix();

        lTransform.Translate({ lHOffset, 0, 0 });
        mModels[7] = lTransform.GetMatrix();

        lTransform.Translate({ lHOffset, 0, 0 });
        mModels[8] = lTransform.GetMatrix();

    }

    mModels[0] = mMainTransform.GetMatrix();

    mViewport->Activate();
    glClearColor(mBackgroundColor.R, mBackgroundColor.G, mBackgroundColor.B, mBackgroundColor.A);

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    mMatricesUbo->UpdateGpu();
    mModelsUbo->UpdateGpu();
    mColorsUbo->UpdateGpu();

    {
        //Mémoire pour un Node: 6 * (3 * 4 + 1) = 78 octets 
        //Mémoire avec liste: 4 * (3 * 4 + 1) + 6 * 2 = 64 octets 
    }

    if (mRenderEdges)
    {
        RenderItem(mLabyrinthShader, mRenderableLabyrinth);
    }

    if (mRenderCells)
    {
        RenderItem(mNodeShader, mRenderableNodes);
    }

    if (mRenderPath)
    {
        RenderItem(mPathShader, mRenderableLongestPath);
    }

    if (mShowWireCube)
    {
        Binder lShaderBinder(*mNodeShader);
        mCubeMesh->Render();
    }

    glDisable(GL_BLEND);
}

void BalyrinthGeneratorWindow::Resize(const Vector2i pSize)
{
    mViewport->SetSize(pSize);
    mMatrices[0].SetOrthographicProjection(0, pSize.Width, 0, pSize.Height, .2f, 50000.f);
}

void BalyrinthGeneratorWindow::RegenerateLabyrinth()
{
    if (!mKeepSeed)
    {
        std::random_device lRandomDevice;

        mSeed.LoDW = (uint64_t(lRandomDevice()) << 32) + lRandomDevice();
        mSeed.HiDW = (uint64_t(lRandomDevice()) << 32) + lRandomDevice();
    }

    mLabyrinthStepper.InitiateGeneration(&mSeed);
    mStartingPointIndex = INVALID_NODE_INDEX;
}

void BalyrinthGeneratorWindow::ProcessImGui()
{
    if (mIsControlWindowVisible)
    {
        if (ImGui::Begin("Control", &mIsControlWindowVisible))
        {
            bool lChanged = false;

            bool lDrawParamChanged = false;
            bool lUpdateMazeDrawerParameters = false;

            if (ImGui::CollapsingHeader("Maze Drawer"))
            {
                lUpdateMazeDrawerParameters |= ImGui::Checkbox("Contiguous Draw", &mContiguousDraw);
                //lUpdateMazeDrawerParameters |= ExecuteCombobox("Shape Mode", mShapeModes);
                lUpdateMazeDrawerParameters |= ExecuteCombobox("Node Shape", mNodeShapeModes);
                if (lUpdateMazeDrawerParameters)
                {//HACK
                    //mMazeGeometryParameters.ViewOffset = { 0, 0 };
                    //mLabyrinthStepper.SetUpdateListener(mShapeDrawModes.Item());
                    mMazeDrawer->SetParameters({ mContiguousDraw/*mShapeModes.Item()*/, mNodeShapeModes.Item() });
                    lDrawParamChanged = true;
                }
            }

            lChanged |= ExecuteCombobox("Shape", mShapeGenerators);

            if (ImGui::CollapsingHeader("Generation Parameters"))
            {
                lChanged |= ExecuteCombobox("Room Select", mRoomSelectMode);
                lChanged |= ExecuteCombobox("Backtrack", mBacktrackModes);
                lChanged |= ExecuteCombobox("Compute Direction", mComputeDirectionModes);
                lChanged |= ImGui::DragInt("Corridor Min Length", &mCorridorMinLength);
                lChanged |= ImGui::DragInt("Corridor Max Length", &mCorridorMaxLength);
            }

            lChanged |= ImGui::DragInt("Width", &mMazeGeometryParameters.Width, .2f, 1, 300);
            lChanged |= ImGui::DragInt("Height", &mMazeGeometryParameters.Height, .2f, 1, 300);
            ImGui::DragFloat("C/Second", &mConnectionPerSecond, .1f, .1f, 1000000.f);

            ImGui::Checkbox("Show Cells", &mRenderCells);
            ImGui::Checkbox("Show Edges", &mRenderEdges);
            ImGui::Checkbox("Show Path", &mRenderPath);

            lDrawParamChanged |= ImGui::DragFloat("Node Width", &mMazeGeometryParameters.NodeWidth, 0.005f, .005f, 1.f);
            lDrawParamChanged |= ImGui::DragFloat("Edge Width", &mMazeGeometryParameters.EdgeWidth, 0.005f, .005f, 2.f);

            if (ImGui::CollapsingHeader("Colors"))
            {
                ImGui::ColorEdit4("Background Color", (&mBackgroundColor.R));
                ImGui::ColorEdit4("Lines Color", (&mColors[0].R));

                for (size_t i = 1; i < 9; ++i)
                {
                    std::string lNumber = std::to_string(i);

                    ImGui::ColorEdit4(("Node Color " + lNumber).c_str(), (&mColors[i].R));
                }
                ImGui::ColorEdit4("Path Color", (&mColors[9].R));
            }

            ImGui::Checkbox("Show neighbors", &mShowNeighbors);

            std::random_device lRandomDevice;

            float lItemWidth = ImGui::GetColumnWidth() / 3.f;
            ImGui::SetNextItemWidth(lItemWidth);
            ImGui::DragScalar("##SeedHW", ImGuiDataType_U64, &mSeed.HiDW, (uint64_t(lRandomDevice()) << 22) + lRandomDevice());
            ImGui::SameLine(0, ImGui::GetCurrentContext()->Style.ItemInnerSpacing.x);
            ImGui::SetNextItemWidth(lItemWidth);
            ImGui::DragScalar("Seed", ImGuiDataType_U64, &mSeed.LoDW, (uint64_t(lRandomDevice()) << 22) + lRandomDevice());

            if (lChanged)
            {
                if (mMazeGeometryParameters.Width != 0 && mMazeGeometryParameters.Height != 0)
                {
                    InternalUpdateTopology();
                    RegenerateLabyrinth();
                }
            }

            if (lDrawParamChanged)
            {
                mLabyrinthStepper.ForceRedraw();
            }

            if (ImGui::Button("Regenerate"))
            {
                RegenerateLabyrinth();
            }

            ImGui::SameLine();
            ImGui::Checkbox("Keep this seed", &mKeepSeed);

            //glGetInteger64v(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, );

            //GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX;

#if 1// debug matrices
            if (ImGui::CollapsingHeader("3D"))
            {
                ImGui::Checkbox("Show Cube", &mShowWireCube);

                for (size_t i = 0; i < 8; ++i)
                {
                    std::string lNumber = std::to_string(i);

                    ImGui::ColorEdit4(("Vertex Color " + lNumber).c_str(), (&mColors[10 + i].R));
                }

                mQuaternion = mMainTransform.GetOrientation();

                if (ImGui::DragFloat4("Quat", &mQuaternion.X, 0.001f))
                {
                    mQuaternion = mQuaternion.Normalized();
                    mMainTransform.SetOrientation(mQuaternion);
                }

                Matrix4f lMatrix = mMainTransform.GetMatrix();

                ImGui::DragFloat4("##Mat0", lMatrix[0].Array(), 0.001f);
                ImGui::DragFloat4("##Mat1", lMatrix[1].Array(), 0.001f);
                ImGui::DragFloat4("##Mat2", lMatrix[2].Array(), 0.001f);
                ImGui::DragFloat4("##Mat3", lMatrix[3].Array(), 0.001f);
            }
#endif
        }

        ImGui::End();
    }

    if (ImGui::Begin("Graphics State"))
    {
        ImGui::Text("%s", mGraphicsState.GetRendererName());

        float lNormalizedMemoryUsed = 0.f;
        const char* lMemoryUsageText = nullptr;

        if (mGraphicsState.GetMemoryUsage(lNormalizedMemoryUsed, &lMemoryUsageText))
        {
            ImGui::ProgressBar(lNormalizedMemoryUsed, ImVec2(-FLT_MIN, 0), lMemoryUsageText);
        }

        //TODO: create something to permit telemetry here
        size_t lVerticesCount =
            mRenderableLabyrinth->GetVertexCount() +
            mRenderableNodes->GetVertexCount() +
            mRenderableLongestPath->GetVertexCount();

        size_t lTriangleCount = lVerticesCount / 3;

        size_t lMemoryUsed = lVerticesCount * sizeof(float) * 3;

        lMemoryUsed /= (1024 * 1024);

        ImGui::Text("%i vertices / %i triangles", lVerticesCount, lTriangleCount);

        ImGui::Text("Graphics Memory used: %i MB", lMemoryUsed);

        ImGui::Text("%.1f fps.", ImGui::GetIO().Framerate);
    }
    ImGui::End();
}

void BalyrinthGeneratorWindow::InternalUpdateTopology()
{
    Parameters lParameters
    {
        2,
        new Parameter[2]
        {
            Parameter { "Width", (void*)mMazeGeometryParameters.Width },
            Parameter { "Height", (void*)mMazeGeometryParameters.Height }
        }
    };

    mShapeProvider = ((ShapeGenerator*)mShapeGenerators.Item())(lParameters);
    mMazeGeometryParameters.Width = lParameters.Params[0].ValueAsInteger32;
    mMazeGeometryParameters.Height = lParameters.Params[1].ValueAsInteger32;

    mLabyrinthStepper.UpdateTopology(mShapeProvider->GetTopology(), mShapeProvider->GetRoomNeighborhood());
    mCurrentTopology = mLabyrinthStepper.GetTopology();
    mLabyrinthStepper.UpdateAlgorithm({ mRoomSelectMode.Item(), mBacktrackModes.Item(), mComputeDirectionModes.Item(), mCorridorMinLength, mCorridorMaxLength });

    delete[] lParameters.Params;
}

bool BalyrinthGeneratorWindow::LoadColorConfiguration()
{
    Color lColors[15];
    memset(lColors, 0xFF, sizeof(lColors));

    LoadSetting(lColors, sizeof(lColors), "Colors.setting");

    bool lValid = false;

    for (size_t i = 0; i < sizeof(lColors) / sizeof(int64_t); ++i)
    {
        if (((int64_t*)(lColors))[i] != 0xFFFFFFFFFFFFFFFF)
        {
            lValid = true;
            break;
        }
    }

    if (lValid)
    {
        mBackgroundColor = lColors[0];

        for (size_t i = 0; i < 14; ++i)
        {
            mColors[i] = lColors[i + 1];
        }

        return true;
    }

    return false;
}

void BalyrinthGeneratorWindow::SaveColorConfiguration()
{
    Color lColors[15];

    lColors[0] = mBackgroundColor;

    for (size_t i = 0; i < 14; ++i)
    {
        lColors[i + 1] = mColors[i];
    }

    SaveSetting(lColors, sizeof(lColors), "Colors.setting");
}

void BalyrinthGeneratorWindow::RenderItem(ShaderProgram* pProgram, Renderable* pRenderable)
{
    Binder lShaderBinder(*pProgram);
    pProgram->UpdateUniform("model_index", 0);
    pRenderable->Draw();

    if (mShowNeighbors)
    {
        for (uint32_t i = 0; i < 8; ++i)
        {
            pProgram->UpdateUniform("model_index", i + 1);

            pRenderable->Draw();
        }
    }
}
