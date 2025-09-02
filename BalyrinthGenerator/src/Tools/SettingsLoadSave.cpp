#include "SettingsLoadSave.h"

#include <iostream>
#include <fstream>

void SaveSetting(const void* pObject, size_t pObjectSize, const char* pFilepath)
{
    std::ofstream lSettingsFile(pFilepath);
    lSettingsFile.write((char*)pObject, pObjectSize);
}

void LoadSetting(void* pObject, size_t pObjectSize, const char* pFilepath)
{
    int32_t lReturnValue = 1;

    std::ifstream lFileStream(pFilepath);

    //Get file size
    uint64_t lCurrentStreamPosition = lFileStream.tellg();
    lFileStream.seekg(0, lFileStream.end);
    size_t lFileSize = lFileStream.tellg();
    lFileStream.seekg(lCurrentStreamPosition, lFileStream.beg);

    //Set settings from file content
    if (lFileSize == pObjectSize)
    {
        lFileStream.read((char*)pObject, pObjectSize);
    }
    else
    {
        std::cout << pFilepath << " and given objet dimensions mismatch : " << pObjectSize << " bytes vs " << lFileSize << " bytes." << std::endl;
    }

    //Close file
    lFileStream.close();
}
