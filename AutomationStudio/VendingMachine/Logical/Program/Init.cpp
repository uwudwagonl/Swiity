/* ============================================================
 * Snackomat Hauptprogramm - Init
 * ========================================================== */

#include <bur/plctypes.h>
#include <string.h>

#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif

void _INIT ProgramInit(void)
{
	USINT i;

	if (Perm_muenzwert == 0) Perm_muenzwert = 10;
	for (i = 0; i < 4; i++) {
		if (Perm_preisCent[i] == 0)     Perm_preisCent[i]     = 150;
		if (Perm_motorLaufzeit[i] == 0) Perm_motorLaufzeit[i] = 2500;
	}

	for (i = 0; i < 4; i++) {
		gFach[i].abstandCm  = 0;
		gFach[i].istLeer    = 0;
		gFach[i].referenzCm = 0;
		gFach[i].preisCent  = Perm_preisCent[i];
	}
	strcpy(gFach[0].name, "Produkt 1");
	strcpy(gFach[1].name, "Produkt 2");
	strcpy(gFach[2].name, "Produkt 3");
	strcpy(gFach[3].name, "Produkt 4");

	for (i = 0; i < 4; i++) {
		Visu.settings.ptMotorX[i] = (INT) Perm_ptMotorX[i];
	}
	Visu.settings.ptMotorVisuInput = (INT) Perm_ptMotorX[0];

	/* Active-LOW Relais: 1 = HIGH = Relais AUS = Motor steht (sicherer Startzustand) */
	for (i = 0; i < 4; i++) doMotor[i] = 1;

	gSel.fachNr = 0;     gSel.aktiv = 0;     gSel.preisCent = 0;
	gPay.modus  = 0;     gPay.eingezahltCent = 0;    gPay.status = 0;
	gNfc.erkannt = 0;    gNfc.inWhitelist = 0;
	gCoin.impulse = 0;   gCoin.summeCent = 0;   gCoin.wertProImpulsCent = Perm_muenzwert;

	Visu.page.setpage = _00_Init;
	Visu.page.actpage = _00_Init;

	stateZahlung = 0;
}
