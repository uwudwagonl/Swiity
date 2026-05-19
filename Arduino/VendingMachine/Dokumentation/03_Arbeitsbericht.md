# Arbeitsbericht: HTLVB_Sweety_Basic → HTLVB_Sweety_new

Projekt Swiity Gruppe C (Berufsschule)
Erweiterung der Schul-Vorlage zum voll funktionsfähigen Snackautomaten
mit ESP32, NFC und TCP/IP-Kommunikation.

---

## 1. Gesamtziel

Die Basis-Vorlage `HTLVB_Sweety_Basic` bietet Grundgerüst für einen
Snackautomaten, steuert aber nur lokal über X20-I/O. Das neue Projekt
`HTLVB_Sweety_new` baut dieses Fundament um zu einem verteilten System
ESP32 ↔ PLC mit Karten-Zahlung und getrennter Hardware für die
Fach-Überwachung.

---

## 2. Hardware-Konfiguration (UNVERÄNDERT übernommen)

Nichts geändert:
- **CPU**: X20CP1382 (IP 192.168.2.20)
- **Panel**: 6PPT50.0502-10A
- **Digital-Input X1**: Münzeinwurf-Lichtschranke (DI01)
- **Digital-Output X3**: 4× Motor (DO01–DO04) + LED (DO05)
- **Analog-Output X20AO2622**: Motor-Geschwindigkeit
- **Automation Runtime**: G4.83

---

## 3. Task-Struktur: ST → ANSI C++ ersetzt

### Entfernt
| Task (Vorlage) | Sprache | Zweck | Ersetzt durch |
|---|---|---|---|
| `visumanager` | Structured Text | Seiten-Navigation, Sprachumschaltung | Program (ANSI C++) |
| `produktausgabe` | Structured Text | Zahlungs-Statemachine, Motorausgabe | Program (ANSI C++) |
| `motorman` | Structured Text | Motor-Tippbetrieb, Settings | Program (ANSI C++) |

### Neu
| Task | Sprache | Zweck |
|---|---|---|
| `Program` | ANSI C++ | Gesamte Geschäftslogik + TCP-Server |
| `Program1` | ANSI C++ | Münz-Zählung mit Entprellung |

**Warum**: Vorgabe des Users — komplette Umstellung auf C++.

---

## 4. Globale Typen (Global.typ) — ERWEITERT

Aus Vorlage beibehalten:
- `eVisuPages` (Seiten-Enum)
- `tVisu_page`, `tVisu_buttons`, `tVisu_settings`, `tVisu`

Neu hinzugefügt (für Snackomat):
- `tFach` – Fach-Info (Abstand, istLeer, Referenz, Preis, Name)
- `tAuswahl` – aktuelle Kundenauswahl
- `tZahlung` – Zahlungsmodus (0=idle, 1=Bar, 2=Karte), Status, Startzeit
- `tNfcInfo` – UID, Whitelist-Status, Zeitstempel
- `tMuenze` – Impulse, Summe, Wert pro Impuls
- `tEspStatus` – ESP32-Verbindung, letzter Ping, Fehlerzähler

Seiten-Enum erweitert:
- Neu: `_20_BarBezahlen`, `_30_KarteBezahlen`, `_40_Ausgabe`, `_50_Danke`, `_90_Fehler`

Button-Struct erweitert:
- Neu: `P10_WaehleBar`, `P10_WaehleKarte`
- Neu: `P20_BestaetigeBar`, `P30_BestaetigeKarte`
- Neu: `PXX_Abbrechen`
- Neu: `P910_Kalibriere1..4`

Settings-Struct erweitert:
- Neu: `muenzwertCent`, `preisCent[0..3]`

---

## 5. Globale Variablen (Global.var) — ERWEITERT

Aus Vorlage beibehalten:
- `Visu`, `aoMotorspeed`, `psMotorspeed`, `diMuenzeinwurf`
- `doMotor[0..3]`, `doLedEin`
- `Perm_ptMotorX[0..3]` (RETAIN)

Neu hinzugefügt:
- `gFach[0..3]` – Fach-Bestandsdaten
- `gSel` – aktuelle Auswahl
- `gPay` – Zahlungsvorgang
- `gNfc` – NFC-Info (kommt vom ESP32)
- `gCoin` – Münzzähler
- `gEsp` – ESP32-Verbindungsstatus
- `do_Relais` – zusätzliches Freigaberelais
- `Perm_preisCent[0..3]`, `Perm_motorLaufzeit[0..3]`, `Perm_muenzwert` (RETAIN)

---

## 6. Visualisierung (Logical/Visu)

### Aus Vorlage beibehalten
- Layout-Basis 800×480 WVGA
- Seiten `_00_Init`, `_10_Main`, `_900_Settings`, `_910_Settings1`
- VCShared, Fonts, Bitmaps, Layers, Styles komplett unverändert
- HotSpots in den 4 Ecken von `_10_Main` (technisch noch da, ohne Funktion)

### Neu
- **5 neue Seiten** als XML-Skelette (Widgets manuell in AS hinzuzufügen):
  - `_20_BarBezahlen.page` (Münzzähler + Bestätigen)
  - `_30_KarteBezahlen.page` (Karte auflegen + NFC-Status)
  - `_40_Ausgabe.page` (Motor-Fortschritt)
  - `_50_Danke.page` (Dankeschön nach Ausgabe)
  - `_90_Fehler.page` (Fehlermeldung)
- Registrierung aller 9 Seiten in `Visu/Package.vcp`
- `DefaultPage` auf `_00_Init` (unverändert)
- `PageChangeDatapoint` auf `Visu.page.setpage` (aus Vorlage)

---

## 7. Hardware-Schnittstelle (IoMap.iom) — UNVERÄNDERT

```
::aoMotorspeed AT %QW."X20AO2622".AnalogOutput01;
::diMuenzeinwurf AT %IX."X1".DigitalInput01;
::doMotor[0..3] AT %QX."X3".DigitalOutput01..04;
doLedEin AT %QX."X3".DigitalOutput05;
```

1:1 aus Vorlage übernommen.

---

## 8. Logik-Funktionalität

### Aus Vorlage beibehalten (neu in C++ geschrieben)

| Funktion | Vorlage (ST) | Neu (ANSI C++) |
|---|---|---|
| Startscreen mit Timer | `visumanager: tStartScreen 5s` | `Cyclic.cpp: jetztMs >= 3000` |
| Seitenwechsel Settings ↔ Main | `tOpenSettings TON` | Lang-Druck 2s auf `PXX_GotoSettings` |
| Settings-Unterseiten durchblättern | `PXX_SettingsNextPage/Prev` | gleich, via `Cyclic.cpp` |
| Motor-Tippbetrieb in Settings | `motorman: P910_MoveMotor` | `Cyclic.cpp` Settings-Sektion |
| Motor-Laufzeit pro Fach speichern | `Perm_ptMotorX[]` | `Perm_ptMotorX[]` + `Perm_motorLaufzeit[]` |
| Sprachumschaltung de/en | `visumanager` Text-Swap | `Cyclic.cpp` Sprachblock |
| Produkt-Texteingabe | `Text_Selection_Visu[]` | gleich + schreibt zusätzlich `gFach[i].name` |
| Münz-Zählung mit Entprellung | `produktausgabe: tVzgMuenzeinwurf` | `Program1/Cyclic.cpp` mit 20 ms Entprellung |
| Direkte Motor-Ausgabe | `produktausgabe: doMotor[i]` | `Cyclic.cpp` Fallback wenn ESP32 offline |
| Zustandsautomat Zahlung | `sStepAusgabe CASE` | `stateZahlung switch-case` |

### Komplett NEU (nicht in Vorlage vorhanden)

| Feature | Code-Stelle |
|---|---|
| **TCP-Server auf Port 8000** | `Cyclic.cpp: stateTcp` + AsTCP-Library |
| **ASCII-Protokoll Parser** (READY, OK:n, DONE:n, LEER:n, VOLL:n, DIST:n, STATUS:n, NFC:UID, PING, ERR:…) | `verarbeiteZeile()` |
| **Bar/Karte-Auswahl** | `stateZahlung 1 → 2 (Bar) oder 3 (Karte)` |
| **NFC-Karten-Flow** | `stateZahlung 3`: wartet auf `gNfc.inWhitelist` |
| **Motor-Trigger via TCP** | `stateZahlung 6`: sendet `M:n` an ESP32 |
| **Fallback direkt doMotor** | falls `gEsp.verbunden == 0` |
| **Watchdog-Timeout für Motor** | 8 s hartes Safety-Timeout |
| **ESP32-Heartbeat-Monitoring** | `gEsp.letzterPingMs + TIMEOUT_ESP_HEARTBEAT_MS` |
| **Kalibrier-Command an ESP32** | Buttons `P910_Kalibriere1..4` senden `C:n` |
| **Abbrechen-Button global** | `PXX_Abbrechen` setzt Statemaschine zurück |

---

## 9. Neue Libraries

| Library | Zweck | Herkunft |
|---|---|---|
| **AsTCP** | TCP/IP Socket-Funktionen | aus AS-Installation kopiert |

Ergänzt in `Logical/Libraries/Package.pkg` und `Cpu.sw`.

---

## 10. Konnektivität & OPC-UA

Neu:
- `Physical/Config1/X20CP1382/Connectivity/OpcUA/OpcUaMap.uad` — exponiert alle
  Snackomat-Variablen für Claude-MCP-Tests
- `AccessAndSecurity/UserRoleSystem/User.user` — Admin/password User hinzugefügt
  (für OPC-UA-Auth)

---

## 11. ESP32-Firmware (komplett neu – nicht in Vorlage)

Ordner: `ESP32_Firmware/snackomat/`

| Datei | Inhalt |
|---|---|
| `snackomat.ino` | Hauptdatei, setup/loop, Watchdog |
| `config.h` | Pins, WLAN, TCP, Whitelist |
| `hcsr04_sensor.h/.cpp` | Ultraschall-Messung, Median aus 3, Round-Robin |
| `motor_control.h/.cpp` | DC-Motor via Relais, nicht-blockierend, per-Fach-Laufzeit |
| `sps_communication.h/.cpp` | TCP-Client, Reconnect, PING/PONG, Fehler-Codes |
| `fach_manager.h/.cpp` | Orchestriert 4 Fächer, LEER/VOLL-Detection |
| `preferences_manager.h/.cpp` | NVS-Speicher für Kalibrierung + Motorlaufzeit |
| `nfc_reader.h/.cpp` | PN532 I2C, Whitelist-Prüfung |

---

## 12. Dokumentation

Neu:
- `01_Verkabelung.pdf` – Stückliste, Pin-Tabellen, Inbetriebnahme
- `02_Visu_Anleitung.pdf` – Schritt-für-Schritt Widget-Anleitung für AS
- `03_Arbeitsbericht.md` – dieses Dokument
- `README.md` im ESP32_Firmware-Ordner

---

## 13. Zusammenfassung: Was ist wirklich neu?

**Aus Vorlage komplett übernommen**: Hardware-Konfig, VC4-Design-System
(Bitmaps, Fonts, Styles, Layer), Produktfach-Grundidee,
Münzzähler-Prinzip, Sprach-Umschaltung, Settings-Architektur.

**In andere Sprache portiert**: alle 3 ST-Tasks → ANSI C++.

**Hinzugefügt**:
1. Distribuierte Architektur mit ESP32 über TCP/IP
2. Kartenzahlung via PN532 NFC + Whitelist
3. Zwei-Wege-Zahlung (Bar oder Karte) mit eigenen Visu-Seiten
4. Bestandsüberwachung pro Fach (HC-SR04 LEER/VOLL)
5. OPC-UA-Freigabe für MCP-basierte Tests
6. Robustheit: Heartbeat, Watchdog, Safety-Timeouts, Fallback-Modus

**Unverändert**: Alles was die Schule physisch vorgegeben hat
(CPU-Typ, I/O-Module, Panel, Ethernet-IP).
