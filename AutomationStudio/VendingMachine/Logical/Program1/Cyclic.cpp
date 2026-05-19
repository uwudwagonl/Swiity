/* ============================================================
 * Muenzzaehlung (Program1) - Cyclic
 *
 * Liest diMuenzeinwurf (X20 DI), erkennt steigende Flanken und
 * addiert pro Impuls den Wert aus gCoin.wertProImpulsCent zu
 * gCoin.summeCent.
 * ========================================================== */

#include <bur/plctypes.h>

#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif

unsigned long bur_heap_size = 0xFFFF;

void _CYCLIC ProgramCyclic(void)
{
	/* einfacher Zyklus-Zaehler (10 ms pro Tick) */
	static UDINT counter = 0;
	counter += 10;

	/* Steigende Flanke an Lichtschranke (=diMuenzeinwurf)? */
	if (diMuenzeinwurf && !lastInput) {
		/* Entprellung: nur zaehlen wenn > entprellungMs seit letzter Flanke */
		if ((counter - lastEdgeMs) > entprellungMs) {
			gCoin.impulse++;
			gCoin.summeCent       += gCoin.wertProImpulsCent;
			gCoin.letzterImpulsMs  = counter;
			lastEdgeMs             = counter;
		}
	}
	lastInput = diMuenzeinwurf;
}
