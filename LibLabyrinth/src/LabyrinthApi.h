#pragma once

#ifdef WIN32
#ifdef LABYRINTH_EXPORTS
#define LABYRINTH_API __declspec(dllexport)
#else
#define LABYRINTH_API __declspec(dllimport)
#endif
#else
#define LABYRINTH_API 
#endif
