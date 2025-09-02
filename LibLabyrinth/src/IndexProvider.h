#pragma once

#include <queue>
#include <deque>
#include <list>
#include <cstdlib>

#include "SeededRandGen.h"

struct IndexProvider
{
	//push index to container
	virtual void InsertIndex(uint32_t pIndex) = 0;

	//pop index from container
	virtual uint32_t GetInsertedIndex() = 0;

	//check container emptiness
	virtual bool Empty() = 0;

	//clear container
	virtual void Clear() = 0;

	SeededRandGen* mRandGen = nullptr;
};

struct QueueIndexProvider :public IndexProvider
{
	std::queue<uint32_t> Queue;

	//push index to container
	void InsertIndex(uint32_t pIndex) override
	{
		Queue.push(pIndex);
	}

	//pop index from container
	uint32_t GetInsertedIndex() override
	{
		uint32_t lReturnValue = Queue.front(); Queue.pop();

		return lReturnValue;
	}

	//check container emptiness
	bool Empty() override
	{
		return Queue.empty();
	}

	void Clear() override
	{
		Queue = std::queue<uint32_t>();
	}
};

struct StackIndexProvider :public IndexProvider
{
	std::deque<uint32_t> Stack;

	//push index to container
	void InsertIndex(uint32_t pIndex) override
	{
		Stack.push_back(pIndex);
	}

	//pop index from container
	uint32_t GetInsertedIndex() override
	{
		uint32_t lReturnValue = Stack.back(); Stack.pop_back();

		return lReturnValue;
	}

	//check container emptiness
	bool Empty() override
	{
		return Stack.empty();
	}

	void Clear() override
	{
		Stack = std::deque<uint32_t>();
	}
};

struct RandomIndexProvider :public IndexProvider
{
	std::vector<uint32_t> InsertedIndices;

	//push index to container
	void InsertIndex(uint32_t pIndex) override
	{
		InsertedIndices.push_back(pIndex);
	}

	//pop index from container
	uint32_t GetInsertedIndex() override
	{
		uint32_t lRandomPosition = mRandGen->GenerateNext() % InsertedIndices.size();

		uint32_t lReturnValue = InsertedIndices[lRandomPosition];

		InsertedIndices.erase(InsertedIndices.begin() + lRandomPosition);

		return lReturnValue;
	}

	//check container emptiness
	bool Empty() override
	{
		return InsertedIndices.empty();
	}

	void Clear() override
	{
		InsertedIndices.clear();
	}
};
