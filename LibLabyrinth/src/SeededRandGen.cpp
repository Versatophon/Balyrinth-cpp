#include "SeededRandGen.h"

#include <random>

#include <pcg_random.hpp>

struct SeededRandGenID
{
    pcg64_fast RandGen;
    Seed Seed;
};

SeededRandGen::SeededRandGen(const Seed pSeed):
    mID(new SeededRandGenID)
{
    SetSeed(pSeed);
}

SeededRandGen::~SeededRandGen()
{
    delete mID;
}

void SeededRandGen::SetSeed(const Seed pSeed)
{
    mID->Seed = pSeed;
    ResetSeed();
}

Seed SeededRandGen::GetSeed() const
{
    return mID->Seed;
}

void SeededRandGen::ResetSeed()
{
    mID->RandGen = pcg64_fast(*(pcg_extras::pcg128_t*)&mID->Seed);
}

void SeededRandGen::NewRandomSeed()
{
    std::random_device lRandomDevice;
    SetSeed({ (uint64_t(lRandomDevice()) << 32) + lRandomDevice(), (uint64_t(lRandomDevice()) << 32) + lRandomDevice() });
}

uint64_t SeededRandGen::GenerateNext() const
{
    return mID->RandGen();
}
