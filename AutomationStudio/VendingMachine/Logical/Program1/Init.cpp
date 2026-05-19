/* ============================================================
 * Muenzzaehlung (Program1) - Init
 * ========================================================== */
#include <bur/plctypes.h>

#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif

void _INIT ProgramInit(void)
{
	lastInput     = 0;
	lastEdgeMs    = 0;
}
