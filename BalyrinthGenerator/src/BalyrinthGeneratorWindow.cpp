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

#include "GL/Shader.h"
#include "GL/Tex.h"
#include "GL/Viewport.h"
#include "GL/Buffers.h"

#include "Maths/FMat4.h"

#include "Resources/InlineShaders.h"

#include "Drawers/ContiguousMazeDrawer.h"
#include "Drawers/ShapeMazeDrawer.h"

#include "Tools/SettingsLoadSave.h"

//TODO: utiliser une table d'indirection contenant les coordonn�es normalis�es de chaque node, avec des g�n�rateurs
//TODO: ajouter un générateur de nombres aléatoires avec un comportement prédictible
//TODO: ajouter une fonctionnalité pour sérialiser les topologies

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

std::vector<Vec2>& BalyrinthGeneratorWindow::GetNodePositions()
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

    mMatricesUbo = new Ubo(sizeof(FMat4) * 2, "matrices");
    mMatrices = (FMat4*)mMatricesUbo->GetMemory();

    mModelsUbo = new Ubo(sizeof(FMat4) * 256, "models");
    mModels = (FMat4*)mModelsUbo->GetMemory();

    mColorsUbo = new Ubo(sizeof(Color) * 6, "colors");
    mColors = (Color*)mColorsUbo->GetMemory();

    if (!LoadColorConfiguration())
    {//if conf read fails
        mColors[0] = { 0.f, 1.f, 0.f, 1.f };
        mColors[1] = { 0, float(0x55) / float(0xFF), 0, float(0xFF) / float(0xFF) };
        mColors[2] = { 0, float(0xAA) / float(0xFF), 0, float(0xFF) / float(0xFF) };
        mColors[3] = { 0, float(0xFF) / float(0xFF), 0, float(0xFF) / float(0xFF) };
        mColors[4] = { float(0xFF) / float(0xFF), 0, 0, float(0xFF) / float(0xFF) };
        mColors[5] = { float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF), float(0xFF) / float(0xFF) };
    }

    mMatrices[0] = FMat4::Id;
    mMatrices[1] = FMat4::Id;

    mModels[0] = FMat4::Id;

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

    mShader->Use();
    mShader->UpdateUniform("model_index", 0);
    mShader->Deuse();

    mShader->LinkUbo(mMatricesUbo);
    mShader->LinkUbo(mModelsUbo);
    mShader->LinkUbo(mColorsUbo);


    //Initialize edges geometry
    mLabyrinthVao = new Vao;

    mLabyrinthVBufs = new ArrayBuffer * [2];

    uint32_t lVerticesCount = (((mMazeGeometryParameters.Height * mMazeGeometryParameters.Width) - 1) + (mMazeGeometryParameters.Height + mMazeGeometryParameters.Width) * 2) * 6;
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


    for (uint32_t i = 0; i < 8; ++i)
    {//neighbors
        ShaderProgram* lShaderProgram = new ShaderProgram;

        lShaderProgram->AttachShader(lVertexShader);
        lShaderProgram->AttachShader(lFragmentShader);

        lShaderProgram->Link();

        lShaderProgram->AddAttribute("vPos", AttributeType::FLOAT);
        lShaderProgram->AddAttribute("vColIndex", AttributeType::INTEGER);
        lShaderProgram->AddUniform("model_index");

        lShaderProgram->Use();
        lShaderProgram->UpdateUniform("model_index", i+1);
        lShaderProgram->Deuse();

        lShaderProgram->LinkUbo(mMatricesUbo);
        lShaderProgram->LinkUbo(mModelsUbo);
        lShaderProgram->LinkUbo(mColorsUbo);

        mShaders.push_back(lShaderProgram);

        mModels[i + 1] = FMat4::Id;

        Vao* lVao = new Vao;

        lVao->Init(lShaderProgram, mLabyrinthVBufs);

        mVaos.push_back(lVao);
    }

    delete lVertexShader;
    delete lFragmentShader;

    std::cout << "Array buffer size " << lVerticesCount * sizeof(float) * 3 << std::endl;

    mCurrentPositionInBuffer = 0;
    mVerticesToUpload.clear();

    Resize(IVec2{ (int32_t)GetWidth(), (int32_t)GetHeight() });

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
            //mModels[0]._03 = GetWidth()/2;
            //mModels[0]._13 = GetHeight()/2;
            break;

        default:
            break;
        }
        break;

    case SDL_EVENT_MOUSE_MOTION:
        if (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_LMASK)
        {
            //TODO: should we move camera instead of data
            //mModels[0]._03 += pEvent->motion.xrel;
            //mModels[0]._13 -= pEvent->motion.yrel;
        }
        break;
    case SDL_EVENT_MOUSE_WHEEL:
    {
        float lZoomFactor = 1.2f;
        //std::cout << pEvent->motion.x << ";" << pEvent->motion.y << std::endl;
        //if (mMazeGeometryParameters.CellWidth * pow(lZoomFactor, pEvent->motion.x) >= 2
        //    && mMazeGeometryParameters.LineWidth * pow(lZoomFactor, pEvent->motion.x) >= 0.5f)
        //{
        //    mMazeGeometryParameters.PointWidth *= pow(lZoomFactor, pEvent->motion.x);
        //    mMazeGeometryParameters.CellWidth *= pow(lZoomFactor, pEvent->motion.x);
        //    mMazeGeometryParameters.LineWidth *= pow(lZoomFactor, pEvent->motion.x);
        //    mLabyrinthStepper.ForceRedraw();
        //}
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
        mNodesVBufs[1]->Upload((mMazeGeometryParameters.Height * mMazeGeometryParameters.Width) * 6 * sizeof(uint8_t) * 3, mNodesNeigborCount.data());

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

        mModels[1]._03 = mModels[0]._03 + lHOffset;
        mModels[1]._13 = mModels[0]._13;

        mModels[2]._03 = mModels[0]._03 + lHOffset;
        mModels[2]._13 = mModels[0]._13 + lVOffset;

        mModels[3]._03 = mModels[0]._03;
        mModels[3]._13 = mModels[0]._13 + lVOffset;

        mModels[4]._03 = mModels[0]._03 - lHOffset;
        mModels[4]._13 = mModels[0]._13 + lVOffset;

        mModels[5]._03 = mModels[0]._03 - lHOffset;
        mModels[5]._13 = mModels[0]._13;

        mModels[6]._03 = mModels[0]._03 - lHOffset;
        mModels[6]._13 = mModels[0]._13 - lVOffset;

        mModels[7]._03 = mModels[0]._03;
        mModels[7]._13 = mModels[0]._13 - lVOffset;

        mModels[8]._03 = mModels[0]._03 + lHOffset;
        mModels[8]._13 = mModels[0]._13 - lVOffset;
    }

    mViewport->Activate();
    glClearColor(mBackgroundColor.R, mBackgroundColor.G, mBackgroundColor.B, mBackgroundColor.A);

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    mMatricesUbo->UpdateGpu();
    mModelsUbo->UpdateGpu();
    mColorsUbo->UpdateGpu();

    mShader->Use();

    {
        if (mRenderEdges)
        {
            mLabyrinthVao->Bind();
            glDrawArrays(GL_TRIANGLES, 0, mCurrentPositionInBuffer / (sizeof(float)*3));
            mLabyrinthVao->Debind();
        }

        if (mRenderCells)
        {
            mNodesVao->Bind();
            glDrawArrays(GL_TRIANGLES, 0, mCurrentPositionInNodesBuffer / (sizeof(float) * 3));
            mNodesVao->Debind();
        }

        if (mRenderPath)
        {
            mPathVao->Bind();
            glDrawArrays(GL_TRIANGLES, 0, mPathVertexCount);
            mPathVao->Debind();
        }

        if (mShowNeighbors)
        {
            for (uint32_t i = 0; i < 8; ++i)
            {
                mShaders[i]->Use();
                mVaos[i]->Bind();
                glDrawArrays(GL_TRIANGLES, 0, mCurrentPositionInBuffer / (sizeof(float) * 3));
                mVaos[i]->Debind();
                mShaders[i]->Deuse();
            }
        }
        
    }

    mShader->Deuse();

    glDisable(GL_BLEND);
}

void BalyrinthGeneratorWindow::Resize(const IVec2 pSize)
{
    mViewport->SetSize(pSize);
    mMatrices[0] = GenOrthographic(-10 + 0,-10 + pSize.Width/30.f, -10 + 0, -10 + pSize.Height/30.f, .2f, 500.f);
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
    mStartingPointIndex = UINT32_MAX;
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
    Color lColors[7];
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

        for (size_t i = 0; i < 6; ++i)
        {
            mColors[i] = lColors[i + 1];
        }

        return true;
    }

    return false;
}

void BalyrinthGeneratorWindow::SaveColorConfiguration()
{
    Color lColors[7];

    lColors[0] = mBackgroundColor;

    for (size_t i = 0; i < 6; ++i)
    {
        lColors[i + 1] = mColors[i];
    }

    SaveSetting(lColors, sizeof(lColors), "Colors.setting");
}
