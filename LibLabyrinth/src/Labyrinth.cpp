#include "Labyrinth.h"

#include "Topology.h"

#include "IndexProvider.h"

#include "SeededRandGen.h"

#include <iostream>

#define NOT_CONNECTED 0
#define ALREADY_IN_SET 1
#define NO_MORE_CONNECTION_POSSIBLE 2

struct Edge
{
	uint32_t FromIndex;
	uint32_t ToIndex;
};

struct LabyrinthStepperId
{
	TopologyUpdaterListener* Listener = nullptr;
	const Topology* mBaseTopology = nullptr;
	Topology* mTopology = nullptr;

	uint32_t mTotalNodeCount = 0;
	uint32_t mConnectedNodeCount = 0;
	uint32_t mLastConnectedNodeIndex = 0;
	uint32_t mFirstIndex = 0;

	SeededRandGen mRandGen;

	std::vector<uint8_t> mGraphColoration;
	std::vector<Edge> mGeneratedEdges;

	IndexProvider* mIndexProvider = nullptr;
	Algorithm mAlgorithm = Algorithm::WallBreakerBloom;

	~LabyrinthStepperId()
	{
		delete mTopology;
	}

	void UpdateTopology(const Topology* pTopology)
	{
		mBaseTopology = pTopology;

		delete mTopology;
		mTopology = new Topology(pTopology->GetSize());
	}

	void UpdateAlgorithm(Algorithm pAlgorithm)
	{
		mAlgorithm = pAlgorithm;

		delete mIndexProvider;
		mIndexProvider = nullptr;

		switch (mAlgorithm)
		{
		case Algorithm::WallBreakerQueueBackTrack:
			mIndexProvider = new QueueIndexProvider();
			break;

		case Algorithm::WallBreakerStackBackTrack:
			mIndexProvider = new StackIndexProvider();
			break;

		case Algorithm::WallBreakerBloom:
		case Algorithm::WallBreakerRandomBackTrack:
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
	}

	void ProcessStep(uint32_t pConnectionCount)
	{
		size_t lLastInsertedIndex = SIZE_MAX;

		bool lDoneSomething = false;

		while (mConnectedNodeCount < mTotalNodeCount && pConnectionCount > 0)
		{
			lDoneSomething = true;;
			const Node* lNode = mBaseTopology->GetNode(mLastConnectedNodeIndex);

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

				if (mAlgorithm != Algorithm::WallBreakerBloom)
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

LabyrinthStepper::LabyrinthStepper(Algorithm pAlgorithm):
	mId(new LabyrinthStepperId)
{
	UpdateAlgorithm(pAlgorithm);
}

LabyrinthStepper::~LabyrinthStepper()
{
	delete mId;
}

void LabyrinthStepper::SetUpdateListener(TopologyUpdaterListener* pListener)
{
	mId->Listener = pListener;
}

void LabyrinthStepper::UpdateTopology(const Topology* pTopology)
{
	mId->UpdateTopology(pTopology);
}

void LabyrinthStepper::UpdateAlgorithm(Algorithm pAlgorithm)
{
	mId->UpdateAlgorithm(pAlgorithm);
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
