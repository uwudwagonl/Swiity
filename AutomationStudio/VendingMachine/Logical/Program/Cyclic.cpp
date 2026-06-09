/* ============================================================
 * Snackomat Hauptprogramm - Cyclic
 *
 * Fach-Status und NFC werden ueber 5 direkte Digitaleingaenge
 * vom ESP32 gelesen:
 *   diEspFach[0..3] : HIGH = VOLL, LOW = LEER
 *   diEspNfc        : HIGH (~2s) = Karte erkannt (beliebige Karte)
 *
 * Motorsteuerung: PLC schaltet doMotor[] direkt nach Ablauf von
 * Perm_motorLaufzeit[].
 * ========================================================== */

#include <bur/plctypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif

unsigned long bur_heap_size = 0xFFFF;

/* Fach-Button gedrueckt? Liefert 1..4 oder 0 */
static UINT pruefeFachButton(void)
{
	if (Visu.buttons.P10_Selection1) { Visu.buttons.P10_Selection1 = 0; return 1; }
	if (Visu.buttons.P10_Selection2) { Visu.buttons.P10_Selection2 = 0; return 2; }
	if (Visu.buttons.P10_Selection3) { Visu.buttons.P10_Selection3 = 0; return 3; }
	if (Visu.buttons.P10_Selection4) { Visu.buttons.P10_Selection4 = 0; return 4; }
	return 0;
}

void _CYCLIC ProgramCyclic(void)
{
	UINT fach;
	UINT idx;
	USINT i;

	jetztMs = jetztMs + 10;

	/* ====================================================
	 * Digitaleingaenge vom ESP32 auslesen
	 * ==================================================== */
	/* Relais (Active-LOW) invertiert: ESP32 HIGH -> Relais AUS -> X20 liest LOW,
	   ESP32 LOW -> Relais EIN -> X20 liest HIGH (24V) */
	for (i = 0; i < 4; i++) {
		gFach[i].istLeer = diEspFach[i] ? 1 : 0;   /* Relais-Inversion: LOW=VOLL, HIGH=LEER */
	}
	gNfc.erkannt     = diEspNfc ? 0 : 1;   /* Relais-Inversion */
	gNfc.inWhitelist = diEspNfc ? 0 : 1;
	gNfc.wartet      = diEspNfc ? 1 : 0;

	/* Status-LED: leuchtet solange die SPS laeuft */
	doLedEin = 1;

	/* Hilfsvariable fuer Visu: Farbwechsel der Progress-Bar */
	gPay.bezahltGenug = (gCoin.summeCent >= gSel.preisCent && gSel.aktiv) ? 1 : 0;

	/* ====================================================
	 * Start-Screen: _00_Init 3 Sekunden zeigen,
	 * dann automatisch auf _10_Main wechseln
	 * ==================================================== */
	if (Visu.page.actpage == _00_Init && jetztMs >= 3000) {
		Visu.page.setpage = _10_Main;
	}

	/* ====================================================
	 * Lang-Druck auf PXX_GotoSettings (2s) -> Settings-Seite
	 * ==================================================== */
	if (Visu.buttons.PXX_GotoSettings) {
		if (!settingsHoldActive) {
			settingsHoldActive  = 1;
			settingsHoldStartMs = jetztMs;
		} else if ((jetztMs - settingsHoldStartMs) >= 2000) {
			Visu.buttons.PXX_GotoSettings = 0;
			settingsHoldActive            = 0;
			Visu.page.setpage             = _900_Settings0;
		}
	} else {
		settingsHoldActive = 0;
	}

	/* Aus Settings raus: Prev/Next-Buttons zwischen 900/910, Abbrechen -> Main */
	if (Visu.buttons.PXX_SettingsNextPage || Visu.buttons.PXX_SettingsPrevPage) {
		Visu.buttons.PXX_SettingsNextPage = 0;
		Visu.buttons.PXX_SettingsPrevPage = 0;
		if (Visu.page.actpage == _900_Settings0)       Visu.page.setpage = _910_Settings1;
		else if (Visu.page.actpage == _910_Settings1)  Visu.page.setpage = _900_Settings0;
	}

	/* ====================================================
	 * Settings-Logik (nur aktiv auf Seite 910)
	 * Motor manuell tippbetreiben + Motorzeit pro Fach speichern
	 * ==================================================== */
	if (Visu.page.actpage == _910_Settings1) {
		USINT selIdx = (USINT) Visu.settings.actMotorSelected;
		if (selIdx > 3) selIdx = 0;

		/* Tippbetrieb: gedrueckt -> Motor an (Active-LOW Relais: 0=EIN, 1=AUS) */
		if (Visu.buttons.P910_MoveMotor) {
			doMotor[selIdx] = 0;
			aoMotorspeed    = psMotorspeed;
		} else {
			doMotor[selIdx] = 1;
			aoMotorspeed    = 0;
		}

		/* Zeiteingabe aus Visu -> Perm speichern */
		if (Visu.settings.ptMotorX_Updated) {
			Visu.settings.ptMotorX_Updated = 0;
			Visu.settings.ptMotorX[selIdx] = Visu.settings.ptMotorVisuInput;
			Perm_ptMotorX[selIdx]          = Visu.settings.ptMotorVisuInput;
			Perm_motorLaufzeit[selIdx]     = (UINT) Visu.settings.ptMotorVisuInput;
		}

		/* Motor-Auswahl gewechselt -> aktuelle Zeit auf Visu schreiben */
		if (Visu.settings.actMotorSelected_Updated) {
			Visu.settings.actMotorSelected_Updated = 0;
			Visu.settings.ptMotorVisuInput = Visu.settings.ptMotorX[selIdx];
		}
	}

	/* ====================================================
	 * Sprach-Umschaltung (de/en)
	 * ==================================================== */
	if (Visu.settings.Language_Updated) {
		for (iTextSel = 0; iTextSel < 4; iTextSel++) {
			if (Visu.settings.Language == 0) {
				strcpy(Visu.settings.Text_Selection_Visu[iTextSel],
				       Visu.settings.Text_Selection_en[iTextSel]);
			} else {
				strcpy(Visu.settings.Text_Selection_Visu[iTextSel],
				       Visu.settings.Text_Selection_de[iTextSel]);
			}
		}
		Visu.settings.Language_Updated = 0;
	}

	/* Texteingabe aus Visu -> in entsprechenden Sprachspeicher */
	for (iTextSel = 0; iTextSel < 4; iTextSel++) {
		if (Visu.settings.Text_Selection_Visu_Updated[iTextSel]) {
			Visu.settings.Text_Selection_Visu_Updated[iTextSel] = 0;
			if (Visu.settings.Language == 0) {
				strcpy(Visu.settings.Text_Selection_en[iTextSel],
				       Visu.settings.Text_Selection_Visu[iTextSel]);
			} else {
				strcpy(Visu.settings.Text_Selection_de[iTextSel],
				       Visu.settings.Text_Selection_Visu[iTextSel]);
			}
			strncpy(gFach[iTextSel].name,
			        Visu.settings.Text_Selection_Visu[iTextSel], 19);
			gFach[iTextSel].name[19] = 0;
		}
	}

	/* ====================================================
	 * Abbrechen-Button
	 * ==================================================== */
	if (Visu.buttons.PXX_Abbrechen) {
		Visu.buttons.PXX_Abbrechen = 0;
		stateZahlung      = 0;
		gPay.modus        = 0;
		gPay.status       = 2;
		gCoin.summeCent   = 0;
		gCoin.impulse     = 0;
		gSel.aktiv        = 0;
		gSel.fachNr       = 0;
		Visu.page.setpage = _10_Main;
	}

	/* ====================================================
	 * Zahlungs-State-Machine
	 * ==================================================== */
	switch (stateZahlung) {

	case 0: /* idle */
		fach = pruefeFachButton();
		if (fach > 0) {
			idx = fach - 1;
			if (gFach[idx].istLeer) {
				gPay.status       = 3;
				Visu.page.setpage = _90_Fehler;
			} else {
				gSel.fachNr       = fach;
				gSel.preisCent    = gFach[idx].preisCent;
				gSel.aktiv        = 1;
				gPay.startZeitMs  = jetztMs;
				stateZahlung      = 1;
				Visu.page.setpage = _15_Zahlungsart;
			}
		}
		break;

	case 1: /* Bar oder Karte? */
		if (Visu.buttons.P10_WaehleBar) {
			Visu.buttons.P10_WaehleBar = 0;
			gPay.modus        = 1;
			stateZahlung      = 2;
			Visu.page.setpage = _20_BarBezahlen;
		} else if (Visu.buttons.P10_WaehleKarte) {
			Visu.buttons.P10_WaehleKarte = 0;
			gPay.modus        = 2;
			stateZahlung      = 3;
			Visu.page.setpage = _30_KarteBezahlen;
		}
		if ((jetztMs - gPay.startZeitMs) > TIMEOUT_ZAHLUNG_MS) {
			stateZahlung      = 0;
			gPay.modus        = 0;
			gPay.status       = 2;
			gCoin.summeCent   = 0;
			gCoin.impulse     = 0;
			gSel.aktiv        = 0;
			gSel.fachNr       = 0;
			Visu.page.setpage = _10_Main;
		}
		break;

	case 2: /* Bar */
		gPay.eingezahltCent = gCoin.summeCent;
		if (gCoin.summeCent >= gSel.preisCent
		    && Visu.buttons.P20_BestaetigeBar) {
			Visu.buttons.P20_BestaetigeBar = 0;
			stateZahlung     = 6;
			lastMotorReqMs   = jetztMs;
		}
		if ((jetztMs - gPay.startZeitMs) > TIMEOUT_ZAHLUNG_MS) {
			stateZahlung      = 0;
			gPay.modus        = 0;
			gPay.status       = 2;
			gCoin.summeCent   = 0;
			gCoin.impulse     = 0;
			gSel.aktiv        = 0;
			gSel.fachNr       = 0;
			Visu.page.setpage = _10_Main;
		}
		break;

	case 3: /* Karte - warten auf NFC-Eingang + Bestaetigung */
		if (gNfc.erkannt && gNfc.inWhitelist
		    && Visu.buttons.P30_BestaetigeKarte) {
			Visu.buttons.P30_BestaetigeKarte = 0;
			stateZahlung   = 6;
			lastMotorReqMs = jetztMs;
		}
		if ((jetztMs - gPay.startZeitMs) > TIMEOUT_ZAHLUNG_MS) {
			stateZahlung      = 0;
			gPay.modus        = 0;
			gPay.status       = 2;
			gCoin.summeCent   = 0;
			gCoin.impulse     = 0;
			gSel.aktiv        = 0;
			gSel.fachNr       = 0;
			Visu.page.setpage = _10_Main;
		}
		break;

	case 6: /* Motor-Start (Active-LOW Relais: 0=EIN) */
		idx = gSel.fachNr - 1;
		if (idx <= 3) doMotor[idx] = 0;
		stateZahlung      = 4;
		lastMotorReqMs    = jetztMs;
		Visu.page.setpage = _40_Ausgabe;
		break;

	case 4: /* Motor laeuft - Timer-basiertes Stoppen */
		idx = gSel.fachNr - 1;
		if (idx <= 3 && (jetztMs - lastMotorReqMs) >= (UDINT) Perm_motorLaufzeit[idx]) {
			doMotor[idx]      = 1;   /* Active-LOW: 1=AUS=Motor steht */
			gPay.status       = 1;
			stateZahlung      = 5;
			lastMotorReqMs    = jetztMs;
			Visu.page.setpage = _50_Danke;
		}
		/* Safety-Timeout */
		if ((jetztMs - lastMotorReqMs) > 8000) {
			if (idx <= 3) doMotor[idx] = 1;   /* Active-LOW: 1=AUS */
			gPay.status       = 3;
			stateZahlung      = 0;
			Visu.page.setpage = _90_Fehler;
		}
		break;

	case 5: /* Danke-Screen 5.5s zeigen */
		if ((jetztMs - lastMotorReqMs) > 5500) {
			stateZahlung      = 0;
			gPay.modus        = 0;
			gPay.status       = 0;
			gCoin.summeCent   = 0;
			gCoin.impulse     = 0;
			gSel.aktiv        = 0;
			gSel.fachNr       = 0;
			Visu.page.setpage = _10_Main;
		}
		break;
	}

	Visu.page.actpage = Visu.page.setpage;
	do_Relais = (stateZahlung == 4);
}
