
TYPE
	eVisuPages :
		(
		_00_Init := 0,
		_10_Main := 10,
		_15_Zahlungsart := 15,
		_20_BarBezahlen := 20,
		_30_KarteBezahlen := 30,
		_40_Ausgabe := 40,
		_50_Danke := 50,
		_90_Fehler := 90,
		_900_Settings0 := 900,
		_910_Settings1 := 910
		);
	tVisu_buttons : 	STRUCT
		P10_Selection1 : USINT;
		P10_Selection2 : USINT;
		P10_Selection3 : USINT;
		P10_Selection4 : USINT;
		P10_StartQuiz : USINT;
		P910_MoveMotor : USINT;
		PXX_GotoSettings : USINT;
		PXX_SettingsNextPage : USINT;
		PXX_SettingsPrevPage : USINT;
		P10_WaehleBar : USINT;
		P10_WaehleKarte : USINT;
		P20_BestaetigeBar : USINT;
		P30_BestaetigeKarte : USINT;
		PXX_Abbrechen : USINT;
	END_STRUCT;
	tVisu_settings : 	STRUCT
		Language : USINT;
		Language_Updated : BOOL;
		actMotorSelected : INT;
		actMotorSelected_Updated : BOOL;
		ptMotorVisuInput : INT;
		ptMotorX : ARRAY[0..3]OF INT;
		ptMotorX_Updated : BOOL;
		Text_Selection_de : ARRAY[0..3]OF STRING[80];
		Text_Selection_en : ARRAY[0..3]OF STRING[80];
		Text_Selection_Visu : ARRAY[0..3]OF STRING[80];
		Text_Selection_Visu_Updated : ARRAY[0..3]OF BOOL;
	END_STRUCT;
	tVisu : 	STRUCT
		page : tVisu_page;
		buttons : tVisu_buttons;
		settings : tVisu_settings;
	END_STRUCT;
	tVisu_page : 	STRUCT
		setpage : eVisuPages;
		actpage : eVisuPages;
	END_STRUCT;
	tFach : 	STRUCT
		abstandCm : UINT;
		istLeer : BOOL;
		referenzCm : UINT;
		preisCent : UINT;
		name : STRING[20];
	END_STRUCT;
	tAuswahl : 	STRUCT
		fachNr : UINT;
		aktiv : BOOL;
		preisCent : UINT;
	END_STRUCT;
	tZahlung : 	STRUCT
		modus : UINT;
		eingezahltCent : UINT;
		status : UINT;
		startZeitMs : UDINT;
		bezahltGenug : BOOL;
	END_STRUCT;
	tNfcInfo : 	STRUCT
		uidHex : STRING[20];
		zeitstempelMs : UDINT;
		erkannt : USINT;
		inWhitelist : USINT;
		wartet : USINT;
	END_STRUCT;
	tMuenze : 	STRUCT
		impulse : UDINT;
		summeCent : UINT;
		wertProImpulsCent : UINT;
		letzterImpulsMs : UDINT;
	END_STRUCT;
END_TYPE
