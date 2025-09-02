#pragma once

#include "LabyrinthApi.h"

#include "Seed.h"

struct SeededRandGenID;

class LABYRINTH_API SeededRandGen
{
public:
    SeededRandGen(const Seed pSeed = Seed());
    ~SeededRandGen();

    //Set initial seed
    void SetSeed(const Seed pSeed);
    
    //Get initial seed
    Seed GetSeed() const;

    //Reinit generator with initial seed
    void ResetSeed();

    //Generate random seed
    void NewRandomSeed();

    //Generate random number from seed
    uint64_t GenerateNext() const;

private:
    SeededRandGenID* mID = nullptr;
};
