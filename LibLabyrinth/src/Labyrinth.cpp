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
	//uint32_t mLastConnectedNodeIndex = 0;

	uint32_t mFirstIndex = 0;
	uint32_t mDirection = 0;
	uint32_t mFromIndex = 0;
	uint32_t mLastDirectionChangedIndex = 0;

	uint32_t mNextDirection = UINT32_MAX;

	StepperState mStepperState = StepperState::Idle;

	SeededRandGen mRandGen;

	std::vector<uint8_t> mGraphColoration;
	std::vector<Edge> mGeneratedEdges;

	IndexProvider* mIndexProvider = nullptr;

	RoomSelectMode mRoomSelectMode = RoomSelectMode::Last;
	BacktrackMode mBacktrackMode = BacktrackMode::Stack;
	DirectionChangeMode mDirectionChangeMode = DirectionChangeMode::Always;
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

	void UpdateAlgorithm(RoomSelectMode pRoomSelectMode, BacktrackMode pBacktrackMode, DirectionChangeMode pDirectionChangeMode)
	{
		mRoomSelectMode = pRoomSelectMode,
		mBacktrackMode = pBacktrackMode;
		mDirectionChangeMode = pDirectionChangeMode;

		//mAlgorithm = pAlgorithm;

		delete mIndexProvider;
		mIndexProvider = nullptr;
#if USE_STATE_MACHINE
		switch (mBacktrackMode)
		{
		case BacktrackMode::Queue:
			mIndexProvider = new QueueIndexProvider();
			break;

		case BacktrackMode::Stack:
			mIndexProvider = new StackIndexProvider();
			break;

		case BacktrackMode::Random:
			mIndexProvider = new RandomIndexProvider();
			break;
		}
#else
		switch (mBacktrackMode)
		{
		case BacktrackMode::Queue:
			mIndexProvider = new QueueIndexProvider();
			break;

		case BacktrackMode::Stack:
			mIndexProvider = new StackIndexProvider();
			break;

		case BacktrackMode::Bloom:
		case BacktrackMode::Random:
			mIndexProvider = new RandomIndexProvider();
			break;
		}
#endif

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

#if USE_STATE_MACHINE
		mStepperState = StepperState::ChooseInitialRoom;
#else
		mTopology->Clear();
		mTotalNodeCount = mBaseTopology->GetSize();
		mConnectedNodeCount = 1;
		mLastConnectedNodeIndex = mRandGen.GenerateNext() % mTotalNodeCount;
		mGraphColoration = std::vector<uint8_t>(mTotalNodeCount, NOT_CONNECTED);

		mIndexProvider->Clear();
		mIndexProvider->InsertIndex(mLastConnectedNodeIndex);
		mGeneratedEdges.clear();

		mGraphColoration[mLastConnectedNodeIndex] = ALREADY_IN_SET;
		mFirstIndex = mLastConnectedNodeIndex;
		Listener->AddFirstNode(mLastConnectedNodeIndex);
#endif
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
				//std::cout << "Direction" << std::endl;
				const size_t lDirectionCount = mRoomNeighborhood->GetDirectionCount();

				std::vector<uint32_t> lConnectableDirections;

				for (size_t i = 0; i < lDirectionCount; ++i)
				{
					uint32_t lNeighborIndex = mRoomNeighborhood->GetNextNode(mFromIndex, i);
					if (lNeighborIndex != INVALID_NODE_INDEX)
					{
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
					mNextDirection = lConnectableDirections[mRandGen.GenerateNext() % lConnectableDirections.size()];
					//Insert

					if (mRoomSelectMode != RoomSelectMode::Fill)
					{
						mIndexProvider->InsertIndex(mFromIndex);
					}
					mLastDirectionChangedIndex = mFromIndex;

					mStepperState = StepperState::WallBreak;
				}

			}
				break;
			case StepperState::WallBreak:
			{
				//std::cout << "Wallbreak" << std::endl;
				uint32_t lNextIndex = mRoomNeighborhood->GetNextNode(mFromIndex, mNextDirection);

				if (lNextIndex == UINT32_MAX)
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

					//mIndexProvider->InsertIndex(lNextIndex);

					mFromIndex = lNextIndex;

					if (mConnectedNodeCount == mTotalNodeCount)
					{
						mTopology->ComputeLongestPath();

						Listener->UpdaterProcessCompleted(mTopology->GetLongestPathLength(), mTopology->GetLongestPathIndices());
						mStepperState = StepperState::Idle;
					}
					else
					{
						mStepperState = StepperState::ChooseNextRoom;
					}

					--pExpectedConnectionCount;
				}
			}
				break;
			case StepperState::ChooseNextRoom:
			{
				//std::cout << "Next Room" << std::endl;
				//TODO:
				//If bloom use previous Index
				if (mRoomSelectMode == RoomSelectMode::Fill)
				{
					mIndexProvider->InsertIndex(mFromIndex);
					mFromIndex = mLastDirectionChangedIndex;
				}

				mStepperState = StepperState::ComputeDirection;
			}
				break;
			case StepperState::Backtrack:
				//std::cout << "Backtrack" << std::endl;
				mFromIndex = mIndexProvider->GetInsertedIndex();
				mStepperState = StepperState::ComputeDirection;
				break;
		}

		return mStepperState != StepperState::Idle && pExpectedConnectionCount > 0;
	}

	void ProcessStep(uint32_t pConnectionCount)
	{
#if USE_STATE_MACHINE
		while (ProcessStepperState(pConnectionCount))
		{
			//ProcessStepperState();
		}

#else
		size_t lLastInsertedIndex = SIZE_MAX;

		bool lDoneSomething = false;

		while (mConnectedNodeCount < mTotalNodeCount && pConnectionCount > 0)
		{
			lDoneSomething = true;
			const Node* lNode = mBaseTopology->GetNode(mLastConnectedNodeIndex);

			//Compute Direction

			const size_t lConnectableNodeCount = lNode->NeighborCount();
			std::vector<uint32_t> lConnectableNodeIndices;

			for (size_t i = 0; i < lConnectableNodeCount; ++i)
			{
				uint32_t lNeighborIndex = lNode->GetNeighborIndex(i);
				if (lNeighborIndex != INVALID_NODE_INDEX)
				{
					if (mGraphColoration[lNeighborIndex] == NOT_CONNECTED)
					{
						lConnectableNodeIndices.push_back(lNeighborIndex);
					}
				}
			}

			if (lConnectableNodeIndices.empty())
			{//dead end
				mLastConnectedNodeIndex = mIndexProvider->GetInsertedIndex();
			}
			else
			{
				//Permit maximum connection per node, because if node has been poped from the container, it cannot be presented again if not repushed here
				if (mLastConnectedNodeIndex != lLastInsertedIndex )
				{
					mIndexProvider->InsertIndex(mLastConnectedNodeIndex);
				}

				uint32_t lNeighborIndex = lConnectableNodeIndices[mRandGen.GenerateNext() % lConnectableNodeIndices.size()];

				mTopology->ConnectNodes(mLastConnectedNodeIndex, lNeighborIndex);
				++mConnectedNodeCount;

				mGeneratedEdges.push_back({ mLastConnectedNodeIndex, lNeighborIndex });
				Listener->AddEdge(mLastConnectedNodeIndex, lNeighborIndex);

				//if (mAlgorithm != Algorithm::WallBreakerBloom)
				if (mBacktrackMode != BacktrackMode::Bloom)
				{
					mLastConnectedNodeIndex = lNeighborIndex;
				}

				mIndexProvider->InsertIndex(lNeighborIndex);
				lLastInsertedIndex = lNeighborIndex;

				mGraphColoration[lNeighborIndex] = ALREADY_IN_SET;

				pConnectionCount--;
			}
		}

		if (lDoneSomething && mConnectedNodeCount == mTotalNodeCount)
		{
			mTopology->ComputeLongestPath();
			
			Listener->UpdaterProcessCompleted(mTopology->GetLongestPathLength(), mTopology->GetLongestPathIndices());
		}
#endif
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

LabyrinthStepper::LabyrinthStepper(RoomSelectMode pRoomSelectMode, BacktrackMode pBacktrackMode, DirectionChangeMode pDirectionChangeMode):
	mId(new LabyrinthStepperId)
{
	UpdateAlgorithm(pRoomSelectMode, pBacktrackMode, pDirectionChangeMode);
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

void LabyrinthStepper::UpdateAlgorithm(RoomSelectMode pRoomSelectMode, BacktrackMode pBacktrackMode, DirectionChangeMode pDirectionChangeMode)
{
	mId->UpdateAlgorithm(pRoomSelectMode, pBacktrackMode, pDirectionChangeMode);
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
