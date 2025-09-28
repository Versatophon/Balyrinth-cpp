#pragma once

#include "LabyrinthApi.h"

#include <cstdint>

enum class Algorithm
{
	WallBreakerStackBackTrack,
	WallBreakerQueueBackTrack,
	WallBreakerRandomBackTrack,
	WallBreakerBloom,
};

class Topology;
struct LabyrinthStepperId;
struct Seed;

class TopologyUpdaterListener
{
public:
	virtual void AddFirstNode(uint32_t pNodeIndex) = 0;
	virtual void AddEdge(uint32_t pNodeIndex0, uint32_t pNodeIndex1) = 0;

	virtual void UpdaterProcessCompleted(uint32_t pPathLength, const uint32_t* pPathIndices) = 0;
};

class LABYRINTH_API LabyrinthStepper
{
public:
	LabyrinthStepper(Algorithm pAlgorithm);
	~LabyrinthStepper();

	void SetUpdateListener(TopologyUpdaterListener* pListener);

	void UpdateTopology(const Topology* pTopology);

	void UpdateAlgorithm(Algorithm pAlgorithm);

	void InitiateGeneration(const Seed* pSeed = nullptr);

	void ProcessStep(uint32_t pConnectionCount);

	void ForceRedraw();

	const Topology* GetTopology() const;

	const Topology* GetBaseTopology() const;

private:
	LabyrinthStepperId* mId = nullptr;
};
