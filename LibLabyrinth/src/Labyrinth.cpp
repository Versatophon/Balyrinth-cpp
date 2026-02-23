#include "Labyrinth.h"

#include "Topology.h"
#include "RoomNeighborhood.h"

#include "IndexProvider.h"

#include "SeededRandGen.h"

#include <iostream>
#include <cstddef>

#define NOT_CONNECTED 0
#define ALREADY_IN_SET 1
#define NO_MORE_CONNECTION_POSSIBLE 2

#define USE_STATE_MACHINE 1

struct Edge
{
	uint32_t FromIndex;
	uint32_t ToIndex;
};

enum class StepperState
{
	Idle,
	ChooseInitialRoom,
	ComputeDirection,
	WallBreak,//can be processed multiple times before going to next state, should be the only one
	ChooseNextRoom,//newly added as clasic wallbreaker, previously added as bloom
	Backtrack,
};

struct LabyrinthStepperId
{
	TopologyUpdaterListener* Listener = nullptr;
	const Topology* mBaseTopology = nullptr;
	const RoomNeighborhood* mRoomNeighborhood = nullptr;
	Topology* mTopology = nullptr;

	uint32_t mTotalNodeCount = 0;
	uint32_t mConnectedNodeCount = 0;

	uint32_t mFirstIndex = 0;
	uint32_t mDirection = 0;
	uint32_t mFromIndex = 0;
	uint32_t mLastDirectionChangedIndex = 0;
	uint32_t mLastDirectionUsed = 0;
	int32_t mCorridorRemainingSteps = 0;

	uint32_t mNextDirection = UINT32_MAX;

	StepperState mStepperState = StepperState::Idle;

	SeededRandGen mRandGen;

	std::vector<uint8_t> mGraphColoration;
	std::vector<Edge> mGeneratedEdges;

	IndexProvider* mIndexProvider = nullptr;

	GenerationParameters mGenerationParameters;

	//RoomSelectMode mRoomSelectMode = RoomSelectMode::Last;
	//BacktrackMode mBacktrackMode = BacktrackMode::Stack;
	//DirectionChangeMode mDirectionChangeMode = DirectionChangeMode::Always;
	//Algorithm mAlgorithm = Algorithm::WallBreakerBloom;

	~LabyrinthStepperId()
	{
		delete mTopology;
	}

	void UpdateTopology(const Topology* pTopology, const RoomNeighborhood* pRoomNeighborhood)
	{
		mBaseTopology = pTopology;
		mRoomNeighborhood = pRoomNeighborhood;

		delete mTopology;
		mTopology = new Topology(pTopology->GetSize());
	}

	void UpdateAlgorithm(GenerationParameters pGenerationParameters)
	{
		mGenerationParameters = pGenerationParameters;
		//mRoomSelectMode = pRoomSelectMode,
		//mBacktrackMode = pBacktrackMode;
		//mDirectionChangeMode = pDirectionChangeMode;

		delete mIndexProvider;
		mIndexProvider = nullptr;

		switch (mGenerationParameters.BacktrackMode)
		{
		case Backtrack::Queue:
			mIndexProvider = new QueueIndexProvider();
			break;

		case Backtrack::Stack:
			mIndexProvider = new StackIndexProvider();
			break;

		case Backtrack::Random:
			mIndexProvider = new RandomIndexProvider();
			break;
		}

		if (mIndexProvider != nullptr)
		{
			mIndexProvider->mRandGen = &mRandGen;
		}
	}

	void InitGeneration(const Seed* pSeed)
	{
		if (pSeed != nullptr)
		{
			mRandGen.SetSeed(*pSeed);
		}

		mStepperState = StepperState::ChooseInitialRoom;
	}

	bool ProcessStepperState(uint32_t& pExpectedConnectionCount)
	{
		switch (mStepperState)
		{
			case StepperState::Idle:
				//Nothing to do here
			break;

			case StepperState::ChooseInitialRoom:
				//std::cout << "Init" << std::endl;

				mTopology->Clear();
				mTotalNodeCount = mBaseTopology->GetSize();
				mConnectedNodeCount = 1;
				mFromIndex = mRandGen.GenerateNext() % mTotalNodeCount;
				mGraphColoration = std::vector<uint8_t>(mTotalNodeCount, NOT_CONNECTED);

				mIndexProvider->Clear();
				mIndexProvider->InsertIndex(mFromIndex);
				mGeneratedEdges.clear();

				mGraphColoration[mFromIndex] = ALREADY_IN_SET;

				mFirstIndex = mFromIndex;
				Listener->AddFirstNode(mFirstIndex);

				--pExpectedConnectionCount;
				mStepperState = StepperState::ComputeDirection;

				break;

			case StepperState::ComputeDirection:
			{
				const size_t lDirectionCount = mRoomNeighborhood->GetDirectionCount();

				std::vector<uint32_t> lConnectableDirections;

				for (size_t i = 0; i < lDirectionCount; ++i)
				{
					uint32_t lNeighborIndex = mRoomNeighborhood->GetNextNode(mFromIndex, i);
					if (lNeighborIndex != INVALID_NODE_INDEX)
					{
						if (lNeighborIndex > 30000000)
						{
							std::cout << "we are in trouble" << std::endl;
						}

						if (mGraphColoration[lNeighborIndex] == NOT_CONNECTED)
						{
							lConnectableDirections.push_back(i);
						}
					}
				}

				if (lConnectableDirections.empty())
				{
					mStepperState = StepperState::Backtrack;
				}
				else
				{
					switch (mGenerationParameters.ComputeDirectionMode)
					{
					case ComputeDirection::Any:
						mNextDirection = lConnectableDirections[mRandGen.GenerateNext() % lConnectableDirections.size()];
						break;
					case ComputeDirection::ForceChange:
						if (lConnectableDirections.size() > 1)
						{
							uint32_t lIndex = mRandGen.GenerateNext() % (lConnectableDirections.size() - 1);
							for (uint32_t lDirection : lConnectableDirections)
							{
								if (lDirection != mLastDirectionUsed)
								{
									if (lIndex-- == 0)
									{
										mNextDirection = lDirection;
									}
								}
							}
						}
						else
						{
							mNextDirection = lConnectableDirections[0];
						}
						break;
					}

					//mNextDirection = lConnectableDirections[mRandGen.GenerateNext() % lConnectableDirections.size()];
					mLastDirectionUsed = mNextDirection;
					//Insert

					//if (mGenerationParameters.RoomSelectMode != RoomSelect::Fill)
					{
						mIndexProvider->InsertIndex(mFromIndex);
					}
					mLastDirectionChangedIndex = mFromIndex;


					{//compute next corridor length
						int32_t lCorridorDelta = mGenerationParameters.CorridorMaxLength - mGenerationParameters.CorridorMinLength;
						int32_t lComputedRandom = 0;
						int32_t lCorridorLength = 1;

						if (lCorridorDelta > 0)
						{
							lComputedRandom = mRandGen.GenerateNext() % (lCorridorDelta+1);
						}
						
						lCorridorLength = mGenerationParameters.CorridorMinLength + lComputedRandom;

						if (lCorridorLength > 0)
						{
							mCorridorRemainingSteps = lCorridorLength;
						}
						else
						{
							mCorridorRemainingSteps = 1;
						}
					}

					mStepperState = StepperState::WallBreak;
				}

			}
				break;
			case StepperState::WallBreak:
			{
				uint32_t lNextIndex = mRoomNeighborhood->GetNextNode(mFromIndex, mNextDirection);

				if (lNextIndex == UINT32_MAX || mGraphColoration[lNextIndex] == ALREADY_IN_SET)
				{//unable to connect this way, need to change direction
					mStepperState = StepperState::ComputeDirection;
				}
				else
				{
					mTopology->ConnectNodes(mFromIndex, lNextIndex);
					++mConnectedNodeCount;

					mGeneratedEdges.push_back({ mFromIndex, lNextIndex });
					Listener->AddEdge(mFromIndex, lNextIndex);

					mGraphColoration[lNextIndex] = ALREADY_IN_SET;

					mFromIndex = lNextIndex; 
					mCorridorRemainingSteps--;

					if (mConnectedNodeCount == mTotalNodeCount)
					{
						mTopology->ComputeLongestPath();

						Listener->UpdaterProcessCompleted(mTopology->GetLongestPathLength(), mTopology->GetLongestPathIndices());
						mStepperState = StepperState::Idle;
					}
					else if (mCorridorRemainingSteps <= 0)
					{//go to next direction change, if corridor ended
						mStepperState = StepperState::ChooseNextRoom;
					}

					--pExpectedConnectionCount;
				}
			}
				break;
			case StepperState::ChooseNextRoom:
			{
				if (mGenerationParameters.RoomSelectMode == RoomSelect::Fill)
				{
					mIndexProvider->InsertIndex(mFromIndex);
					mFromIndex = mLastDirectionChangedIndex;
				}

				mStepperState = StepperState::ComputeDirection;
			}
				break;
			case StepperState::Backtrack:
				mFromIndex = mIndexProvider->GetInsertedIndex();

				if (mFromIndex == UINT32_MAX)
				{
					mStepperState = StepperState::Idle;
				}
				else
				{
					mStepperState = StepperState::ComputeDirection;
				}
				break;
		}

		return mStepperState != StepperState::Idle && pExpectedConnectionCount > 0;
	}

	void ProcessStep(uint32_t pConnectionCount)
	{
		while (ProcessStepperState(pConnectionCount))
		{
		}
	}

	void ForceRedraw()
	{
		Listener->AddFirstNode(mFirstIndex);
		for (const Edge& lEdge : mGeneratedEdges)
		{
			Listener->AddEdge(lEdge.FromIndex, lEdge.ToIndex);
		}

		if (mConnectedNodeCount == mTotalNodeCount)
		{
			Listener->UpdaterProcessCompleted(mTopology->GetLongestPathLength(), mTopology->GetLongestPathIndices());
		}
	}
};

LabyrinthStepper::LabyrinthStepper(GenerationParameters pGenerationParameters):
	mId(new LabyrinthStepperId)
{
	UpdateAlgorithm(pGenerationParameters);
}

LabyrinthStepper::~LabyrinthStepper()
{
	delete mId;
}

void LabyrinthStepper::SetUpdateListener(TopologyUpdaterListener* pListener)
{
	mId->Listener = pListener;
}

void LabyrinthStepper::UpdateTopology(const Topology* pTopology, const RoomNeighborhood* pRoomNeighborhood)
{
	mId->UpdateTopology(pTopology, pRoomNeighborhood);
}

void LabyrinthStepper::UpdateAlgorithm(GenerationParameters pGenerationParameters)
{
	mId->UpdateAlgorithm(pGenerationParameters);
}

void LabyrinthStepper::InitiateGeneration(const Seed* pSeed)
{
	mId->InitGeneration(pSeed);
}

void LabyrinthStepper::ProcessStep(uint32_t pConnectionCount)
{
	mId->ProcessStep(pConnectionCount);
}

void LabyrinthStepper::ForceRedraw()
{
	mId->ForceRedraw();
}

const Topology* LabyrinthStepper::GetTopology() const 
{
	return mId->mTopology;
}

const Topology* LabyrinthStepper::GetBaseTopology() const
{
	return mId->mBaseTopology;
}
