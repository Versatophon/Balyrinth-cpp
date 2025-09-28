#pragma once

#ifdef WIN32
#ifdef SIMPLE_MATHS_EXPORTS
#define SIMPLE_MATHS_API __declspec(dllexport)
#else
#define SIMPLE_MATHS_API __declspec(dllimport)
#endif
#else
#define SIMPLE_MATHS_API 
#endif
