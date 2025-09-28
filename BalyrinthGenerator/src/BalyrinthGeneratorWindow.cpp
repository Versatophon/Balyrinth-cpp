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

#include "GL/Shader.h"
#include "GL/Tex.h"
#include "GL/Viewport.h"
#include "GL/Buffers.h"

#include "Resources/InlineShaders.h"

#include "Drawers/ContiguousMazeDrawer.h"
#include "Drawers/ShapeMazeDrawer.h"

#include "Tools/SettingsLoadSave.h"

#include "GL/Binder.h"

//TODO: utiliser une table d'indirection contenant les coordonn�es normalis�es de chaque node, avec des g�n�rateurs
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
    mLabyrinthStepper(LabyrinthStepper(Algorithm::WallBreakerQueueBackTrack)),
    mViewport(new Viewport)
{
    mShapeGenerators = {{{"Squares On Tore", (void*)GenerateSquaresOnToreShape},
                         {"Squares On Rect", (void*)GenerateSquaresOnRectShape}}};
    mShapeDrawModes = {{{"Shape", new ShapeMazeDrawer(*this)},
                        {"Contiguous", new ContiguousMazeDrawer(*this)}}};

    mAlgorithms = { {{"WB Stack Backtrack", Algorithm::WallBreakerStackBackTrack}, 
                     {"WB Queue Backtrack", Algorithm::WallBreakerQueueBackTrack},
                     {"WB Random Backtrack", Algorithm::WallBreakerRandomBackTrack},
                     {"WB Bloom", Algorithm::WallBreakerBloom}}};

    mLabyrinthStepper.SetUpdateListener(mShapeDrawModes.Item());

    {
        mForNodesLut.resize(mMazeGeometryParameters.Height * mMazeGeometryParameters.Width);
        mNodesNeigborCount.resize(mMazeGeometryParameters.Height * mMazeGeometryParameters.Width * 6);

        memset(mNodesNeigborCount.data(), 0, mNodesNeigborCount.size());
    }

    InternalUpdateTopology();
    mLabyrinthStepper.InitiateGeneration(&mSeed);

    mNeighborTransforms.resize(8);

    mMainTransform.Position.Z = -300;
}
 
BalyrinthGeneratorWindow::~BalyrinthGeneratorWindow()
{
    for (std::pair<std::string, TopologyUpdaterListener*> lListener : mShapeDrawModes.mItems)
    {
        delete lListener.second;
    }

    delete mViewport;
}

Shape* BalyrinthGeneratorWindow::GetShape()
{
    return mShapeProvider;
}

std::vector<Vector2f>& BalyrinthGeneratorWindow::GetNodePositions()
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
    return mNodesNeigborCount;
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
    mNodesNeigborCount.resize(mMazeGeometryParameters.Height * mMazeGeometryParameters.Width * 6);

    memset(mNodesNeigborCount.data(), 0, mNodesNeigborCount.size());
}

int32_t BalyrinthGeneratorWindow::Init()
{
    GLenum lreturn = glewInit();

    mMatricesUbo = new Ubo(sizeof(Matrix4f) * 2, "matrices");
    mMatrices = (Matrix4f*)mMatricesUbo->GetMemory();

    mModelsUbo = new Ubo(sizeof(Matrix4f) * 256, "models");
    mModels = (Matrix4f*)mModelsUbo->GetMemory();

    mColorsUbo = new Ubo(sizeof(Color) * 14, "colors");
    mColors = (Color*)mColorsUbo->GetMemory();

    if (!LoadColorConfiguration())
    {//if conf read fails
        mColors[0] = { 0.f, 1.f, 0.f, 1.f };
        mColors[1] = { 0, float(0x55) / float(0xFF), 0, float(0xFF) / float(0xFF) };
        mColors[2] = { 0, float(0xAA) / float(0xFF), 0, float(0xFF) / float(0xFF) };
        mColors[3] = { 0, float(0xFF) / float(0xFF), 0, float(0xFF) / float(0xFF) };
        mColors[4] = { float(0xFF) / float(0xFF), 0, 0, float(0xFF) / float(0xFF) };
        mColors[5] = { float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF) };
        
        mColors[6] = { float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF) };
        mColors[7] = { float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF) };
        mColors[8] = { float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF) };
        mColors[9] = { float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF) };
        mColors[10] = { float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF) };
        mColors[11] = { float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF) };
        mColors[12] = { float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF) };
        mColors[13] = { float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF) };
    }

    mMatrices[0] = Matrix4f::Id;
    mMatrices[1] = Matrix4f::Id;

    mMatrices[1].SetPosition({ 0, 0, -10000 });

    mModels[0] = Matrix4f::Id;

    mShader = new ShaderProgram;

    Shader* lVertexShader = new Shader(ShaderType::VERTEX_SHADER);
    Shader* lFragmentShader = new Shader(ShaderType::FRAGMENT_SHADER);

    lVertexShader->LoadFromString(sViewVertexShSource);
    lFragmentShader->LoadFromString(sViewFragmentShSource);
    mShader->AttachShader(lVertexShader);
    mShader->AttachShader(lFragmentShader);

    mShader->Link();

    mShader->AddAttribute("vPos", AttributeType::FLOAT);
    mShader->AddAttribute("vColIndex", AttributeType::INTEGER);
    mShader->AddUniform("model_index");

    {
        Binder lBinder(*mShader);
        mShader->UpdateUniform("model_index", 0);
    }

    mShader->LinkUbo(mMatricesUbo);
    mShader->LinkUbo(mModelsUbo);
    mShader->LinkUbo(mColorsUbo);

    uint32_t lVerticesCount = (((mMazeGeometryParameters.Height * mMazeGeometryParameters.Width) - 1) + (mMazeGeometryParameters.Height + mMazeGeometryParameters.Width) * 2) * 6;

    //Initialize edges geometry
    mLabyrinthVao = new Vao;

    mLabyrinthVBufs = new ArrayBuffer * [2];
    mLabyrinthVBufs[0] = new ArrayBuffer(lVerticesCount * sizeof(float) * 3, BufferUsage::Dynamic, nullptr);
    mLabyrinthVBufs[1] = new ArrayBuffer(lVerticesCount * sizeof(uint8_t), BufferUsage::Dynamic, nullptr);
    mLabyrinthVao->Init(mShader, mLabyrinthVBufs);

    //Initialize nodes geometry
    mNodesVao = new Vao;

    mNodesVBufs = new ArrayBuffer * [2];
    mNodesVBufs[0] = new ArrayBuffer(lVerticesCount * sizeof(float) * 3, BufferUsage::Dynamic, nullptr);
    mNodesVBufs[1] = new ArrayBuffer(lVerticesCount * sizeof(uint8_t), BufferUsage::Dynamic, nullptr);
    mNodesVao->Init(mShader, mNodesVBufs);

    mPathVao = new Vao;
   
    mPathVBufs = new ArrayBuffer * [2];
    mPathVBufs[0] = new ArrayBuffer(0 * sizeof(float) * 3, BufferUsage::Dynamic, nullptr);
    mPathVBufs[1] = new ArrayBuffer(0 * sizeof(uint8_t), BufferUsage::Dynamic, nullptr);
    mPathVao->Init(mShader, mPathVBufs);

    {

        std::vector<Vector3f> lVertices =
        {
            {-1.f, -1.f, -1.f},//0
            {1.f, -1.f, -1.f},//1

            {-1.f, 1.f, -1.f},//2
            {1.f, 1.f, -1.f},//3

            {-1.f, -1.f, -1.f},//0
            {-1.f, 1.f, -1.f},//2

            {1.f, -1.f, -1.f},//1
            {1.f, 1.f, -1.f},//3

            {-1.f, -1.f, 1.f},//4
            {1.f, -1.f, 1.f},//5

            {-1.f, 1.f, 1.f},//6
            {1.f, 1.f, 1.f},//7

            {-1.f, -1.f, 1.f},//4
            {-1.f, 1.f, 1.f},//6

            {1.f, -1.f, 1.f},//5
            {1.f, 1.f, 1.f},//7


            {-1.f, -1.f, -1.f},//0
            {-1.f, -1.f, 1.f},//4

            {1.f, -1.f, -1.f},//1
            {1.f, -1.f, 1.f},//5

            {1.f, 1.f, -1.f},//3
            {1.f, 1.f, 1.f},//7

            {-1.f, 1.f, -1.f},//2
            {-1.f, 1.f, 1.f},//6


        };

        uint8_t lBaseIndex = 6;

        std::vector<uint8_t> lColIndices =
        {
            uint8_t(lBaseIndex + 0), uint8_t(lBaseIndex + 1), uint8_t(lBaseIndex + 2), uint8_t(lBaseIndex + 3),
            uint8_t(lBaseIndex + 0), uint8_t(lBaseIndex + 2), uint8_t(lBaseIndex + 1), uint8_t(lBaseIndex + 3),
            uint8_t(lBaseIndex + 4), uint8_t(lBaseIndex + 5), uint8_t(lBaseIndex + 6), uint8_t(lBaseIndex + 7),
            uint8_t(lBaseIndex + 4), uint8_t(lBaseIndex + 6), uint8_t(lBaseIndex + 5), uint8_t(lBaseIndex + 7),
            uint8_t(lBaseIndex + 0), uint8_t(lBaseIndex + 4), uint8_t(lBaseIndex + 1), uint8_t(lBaseIndex + 5),
            uint8_t(lBaseIndex + 3), uint8_t(lBaseIndex + 7), uint8_t(lBaseIndex + 2), uint8_t(lBaseIndex + 6),
        };

        mCubeVao = new Vao;

        mCubeVBufs = new ArrayBuffer * [2];
        mCubeVBufs[0] = new ArrayBuffer(24 * sizeof(float) * 3, BufferUsage::Dynamic, lVertices.data());
        mCubeVBufs[1] = new ArrayBuffer(24 * sizeof(uint8_t), BufferUsage::Dynamic, lColIndices.data());
        mCubeVao->Init(mShader, mCubeVBufs);




    }

    delete lVertexShader;
    delete lFragmentShader;

    mMainTransform.SetScale(100.f);

    mCurrentPositionInBuffer = 0;
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
    mLabyrinthStepper.ProcessStep(mConnectionPerFrame);

    ProcessImGui();

    return SDL_APP_CONTINUE;
}

void BalyrinthGeneratorWindow::Quit()
{
    SaveColorConfiguration();

    delete mShader;
    delete mMatricesUbo;
    delete mModelsUbo;
    delete mColorsUbo;
}

void BalyrinthGeneratorWindow::Render()
{
    if (mNeedToCleanGeometry)
    {
        uint32_t lVerticesCount = (((mMazeGeometryParameters.Height * mMazeGeometryParameters.Width) - 1) + (mMazeGeometryParameters.Height + mMazeGeometryParameters.Width) * 2) * 6;
        mLabyrinthVBufs[0]->Upload(lVerticesCount * sizeof(float) * 3, nullptr);

        std::vector<uint8_t> lTempColorIndex(lVerticesCount, 0);
        mLabyrinthVBufs[1]->Upload(lVerticesCount * sizeof(uint8_t), lTempColorIndex.data());

        mNodesVBufs[0]->Upload((mMazeGeometryParameters.Height * mMazeGeometryParameters.Width) * 6 * sizeof(float) * 3, nullptr);
        mNodesVBufs[1]->Upload(mNodesNeigborCount.size() * sizeof(uint8_t), mNodesNeigborCount.data());

        mPathVertexCount = 0;

        mPathVBufs[0]->Upload(0, nullptr);
        mPathVBufs[1]->Upload(0, nullptr);

        mCurrentPositionInBuffer = 0;
        mCurrentPositionInNodesBuffer = 0;
        mNeedToCleanGeometry = false;
    }

    if (!mVerticesToUpload.empty())
    {
        uint32_t lSize = mVerticesToUpload.size() * sizeof(float);
        mLabyrinthVBufs[0]->PartialUpload(mCurrentPositionInBuffer, lSize, mVerticesToUpload.data());
        mCurrentPositionInBuffer += lSize;

        mVerticesToUpload.clear();
    }

    if (!mForNodesVerticesToUpload.empty())
    {
        uint32_t lSize = mForNodesVerticesToUpload.size() * sizeof(float);
        mNodesVBufs[0]->PartialUpload(mCurrentPositionInNodesBuffer, lSize, mForNodesVerticesToUpload.data());
        mCurrentPositionInNodesBuffer += lSize;
            
        //put it in another conditionnal ?
        mNodesVBufs[1]->Upload(mNodesNeigborCount.size() * sizeof(uint8_t), mNodesNeigborCount.data());

        mForNodesVerticesToUpload.clear();
    }

    if (!mForPathVerticesToUpload.empty())
    {
        uint32_t lSize = mForPathVerticesToUpload.size() * sizeof(float);
        mPathVertexCount = mForPathVerticesToUpload.size() / 3;

        std::vector<uint8_t> lTempColorIndex(mForPathVerticesToUpload.size(), 5);

        mPathVBufs[0]->Upload(lSize, mForPathVerticesToUpload.data());
        mPathVBufs[1]->Upload(mForPathVerticesToUpload.size(), lTempColorIndex.data());

        mForPathVerticesToUpload.clear();
    }

    {
        float lHOffset = mMazeGeometryParameters.Width;
        float lVOffset = mMazeGeometryParameters.Height;

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

    Binder lShaderBinder(*mShader);

    {
        mShader->UpdateUniform("model_index", 0);

        //{
        //    Binder lCubeBinder(*mCubeVao);
        //    glDrawArrays(GL_LINES, 0, 24);
        //}

#if 1
        if (mRenderEdges)
        {
            Binder lEdgesBinder(*mLabyrinthVao);
            glDrawArrays(GL_TRIANGLES, 0, mCurrentPositionInBuffer / (sizeof(float)*3));
        }

        if (mRenderCells)
        {
            Binder lCellsBider(*mNodesVao);
            glDrawArrays(GL_TRIANGLES, 0, mCurrentPositionInNodesBuffer / (sizeof(float) * 3));
        }

        if (mRenderPath)
        {
            Binder lPathBinder(*mPathVao);
            glDrawArrays(GL_TRIANGLES, 0, mPathVertexCount);
        }

        if (mShowNeighbors)
        {
            for (uint32_t i = 0; i < 8; ++i)
            {
                //Binder lNeighborShaderBinder(*mShaders[i]);

                mShader->UpdateUniform("model_index", i+1);

                if (mRenderEdges)
                {
                    Binder lEdgesBinder(*mLabyrinthVao);
                    glDrawArrays(GL_TRIANGLES, 0, mCurrentPositionInBuffer / (sizeof(float) * 3));
                }

                if (mRenderCells)
                {
                    Binder lCellsBider(*mNodesVao);
                    glDrawArrays(GL_TRIANGLES, 0, mCurrentPositionInNodesBuffer / (sizeof(float) * 3));
                }

                if (mRenderPath)
                {
                    Binder lPathBinder(*mPathVao);
                    glDrawArrays(GL_TRIANGLES, 0, mPathVertexCount);
                }

                //Binder lNeighborsEdgesBinder(*mVaos[i]);
                //glDrawArrays(GL_TRIANGLES, 0, mCurrentPositionInBuffer / (sizeof(float) * 3));
            }
        }
#endif
    }

    //mShader->Deuse();

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

            ImGui::Text("%f fps.", ImGui::GetIO().Framerate);

            bool lDrawParamChanged = false;
            if (ExecuteCombobox("Shape Draw mode", mShapeDrawModes))
            {//HACK
                //mMazeGeometryParameters.ViewOffset = { 0, 0 };
                mLabyrinthStepper.SetUpdateListener(mShapeDrawModes.Item());
                lDrawParamChanged = true;
            }

            lChanged |= ExecuteCombobox("Shape", mShapeGenerators);

            lChanged |= ExecuteCombobox("Algorithm", mAlgorithms);

            lChanged |= ImGui::DragScalar("Width", ImGuiDataType_::ImGuiDataType_U64, &mMazeGeometryParameters.Width, .2f, &mMin, &mMax);
            lChanged |= ImGui::DragScalar("Height", ImGuiDataType_::ImGuiDataType_U64, &mMazeGeometryParameters.Height, .2f, &mMin, &mMax);
            ImGui::DragScalar("C/Frame", ImGuiDataType_::ImGuiDataType_U64, &mConnectionPerFrame, .2f, &mMin, &mMax);

            ImGui::Checkbox("Show Cells", &mRenderCells);
            ImGui::Checkbox("Show Edges", &mRenderEdges);
            ImGui::Checkbox("Show Path", &mRenderPath);

            lDrawParamChanged |= ImGui::DragFloat("Point Width", &mMazeGeometryParameters.PointWidth, 0.005f, .005f, 1.1f);
            lDrawParamChanged |= ImGui::DragFloat("Line Width", &mMazeGeometryParameters.LineWidth, 0.005f, .005f, 1.1f);

            ImGui::ColorEdit4("Background Color", (&mBackgroundColor.R));
            ImGui::ColorEdit4("Lines Color", (&mColors[0].R));

            for (size_t i = 1; i < 5; ++i)
            {
                std::string lNumber = std::to_string(i);

                ImGui::ColorEdit4(("Node Color " + lNumber).c_str(), (&mColors[i].R));
            }
            ImGui::ColorEdit4("Path Color", (&mColors[5].R));

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
        }

        if (ImGui::Button("Regenerate"))
        {
            RegenerateLabyrinth();
        }

        ImGui::SameLine();
        ImGui::Checkbox("Keep this seed", &mKeepSeed);

        for (size_t i = 0; i < 8; ++i)
        {
            std::string lNumber = std::to_string(i);

            ImGui::ColorEdit4(("Vertex Color " + lNumber).c_str(), (&mColors[6+i].R));
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

        ImGui::End();
    }
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

    mLabyrinthStepper.UpdateTopology(mShapeProvider->GetTopology());
    mCurrentTopology = mLabyrinthStepper.GetTopology();
    mLabyrinthStepper.UpdateAlgorithm(mAlgorithms.Item());

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
