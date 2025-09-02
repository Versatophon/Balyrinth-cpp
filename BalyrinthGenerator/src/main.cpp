#include "BalyrinthGeneratorWindow.h"

#ifdef WIN32
#include <Windows.h>
#endif

#ifdef WIN32
// Use discrete GPU by default.
#ifdef __cplusplus
extern "C" {
#endif
    //http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
    //http://developer.amd.com/community/blog/2015/10/02/amd-enduro-system-for-developers/
    __declspec(dllexport) int32_t AmdPowerXpressRequestHighPerformance = 1;
#ifdef __cplusplus
}
#endif
#endif

#include <cstdlib>

int main(int pArgC, char** pArgV)
{
	BalyrinthGeneratorWindow lWindow;

	lWindow.Execute();

	return EXIT_SUCCESS;
}
