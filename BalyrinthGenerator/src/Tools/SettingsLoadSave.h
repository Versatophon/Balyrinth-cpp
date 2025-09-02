#pragma once

#include <cstddef>//size_t

void SaveSetting(const void* pObject, size_t pObjectSize, const char* pFilepath);
void LoadSetting(void* pObject, size_t pObjectSize, const char* pFilepath);
