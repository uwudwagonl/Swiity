# Snackomat ESP32-Firmware

Firmware für den Snackautomaten von Gruppe C (Swiity, Berufsschule).

## Hardware
- ESP32 DevKit (beliebige Variante mit ≥ 30 Pins)
- 4 × HC-SR04 Ultraschall-Sensor
- 4-Kanal Relais-Modul (active HIGH)
- 4 × DC-Getriebemotor 12 V
- PN532 NFC-Leser (I2C-Modus)
- 5 V Netzteil für ESP32, 12 V Netzteil für Motoren (getrennt!)

## Software-Setup

### 1. Arduino IDE konfigurieren
- Arduino IDE ≥ 2.0
- Board-Manager-URL ergänzen:
  `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
- Board installieren: **esp32** von Espressif
- Board auswählen: *ESP32 Dev Module*

### 2. Libraries installieren (Library Manager)
- **Adafruit PN532** (aktuellste Version)
- **Adafruit BusIO** (wird automatisch mitgezogen)

`WiFi`, `Preferences`, `Wire` und `esp_task_wdt` sind im ESP32-Core enthalten.

### 3. Konfiguration anpassen
Öffne [config.h](snackomat/config.h) und passe an:
```cpp
#define WIFI_SSID      "Snackomat_WLAN"
#define WIFI_PASSWORD  "bitte-aendern"
#define PLC_IP         "192.168.2.20"      // Deine X20CP1382 IP
#define PLC_TCP_PORT   8000
```

NFC-Whitelist (Karten-UIDs) ebenfalls dort pflegen:
```cpp
static const char* NFC_WHITELIST[NFC_WHITELIST_MAX] = {
    "04A1B2C3D4",   // Schuelerkarte 1
    ...
};
```

### 4. Sketch öffnen und flashen
Öffne `snackomat/snackomat.ino` in der Arduino IDE. Klick auf **Upload**.

### 5. Serial-Monitor
Baudrate `115200`. Erwartete Ausgabe nach Boot:
```
================================================
  Snackomat ESP32-Firmware (Swiity Gruppe C)
================================================
[120ms] NVS geoeffnet (Namespace: snackomat)
[135ms] Sensor Fach 1 initialisiert (TRIG=26, ECHO=27)
...
[450ms] WLAN starte zu SSID 'Snackomat_WLAN' (non-blocking)
[5200ms] WLAN verbunden. IP: 192.168.2.99, RSSI: -63 dBm
[5400ms] TCP verbinde zu 192.168.2.20:8000 ...
[5500ms] TCP-Verbindung zur PLC steht
[5500ms] -> PLC: READY
```

## Dateiübersicht

| Datei | Zweck |
|-------|-------|
| [snackomat.ino](snackomat/snackomat.ino) | Hauptdatei, setup() + loop() |
| [config.h](snackomat/config.h) | Pins, Konstanten, WLAN, NFC-Whitelist |
| [hcsr04_sensor.h/.cpp](snackomat/hcsr04_sensor.cpp) | Ultraschall, Median aus 3 Messungen |
| [motor_control.h/.cpp](snackomat/motor_control.cpp) | DC-Motor via Relais, nicht-blockierend |
| [sps_communication.h/.cpp](snackomat/sps_communication.cpp) | TCP-Client zur PLC, Protokoll-Parser |
| [fach_manager.h/.cpp](snackomat/fach_manager.cpp) | Orchestriert Sensoren + Motoren (Round-Robin) |
| [preferences_manager.h/.cpp](snackomat/preferences_manager.cpp) | NVS: Kalibrierung, LEER/VOLL, Laufzeit |
| [nfc_reader.h/.cpp](snackomat/nfc_reader.cpp) | PN532 I2C, Whitelist-Prüfung |

## Protokoll (TCP/IP, ASCII, `\n`-getrennt)

### PLC → ESP32
| Befehl | Bedeutung |
|--------|-----------|
| `M:1`  | Motor Fach 1 starten |
| `C:1`  | Fach 1 kalibrieren (Referenzwert speichern) |
| `S:1`  | Status Fach 1 anfordern |
| `A`    | Status aller Fächer anfordern |
| `P:1,1.50` | Simulierte Zahlung 1,50 € Fach 1 → Motor starten |
| `PING` | Antwort: `PONG` |

### ESP32 → PLC
| Nachricht | Bedeutung |
|-----------|-----------|
| `READY`      | System gebootet/verbunden |
| `OK:1`       | Motor 1 gestartet |
| `DONE:1`     | Motor 1 fertig (nach Laufzeit) |
| `LEER:1` / `VOLL:1` | Bestandsänderung Fach 1 |
| `DIST:1,23`  | Aktueller Abstand Fach 1 in cm |
| `STATUS:1,VOLL` | Antwort auf `S:1` |
| `NFC:04A1B2C3D4,OK` | Karte gescannt, in Whitelist |
| `NFC:04A1B2C3D4,UNBEKANNT` | Karte nicht in Whitelist |
| `PING` | Heartbeat alle 2 s |
| `ERR:WLAN,reconnect` | Fehlermeldung mit Quelle + Detail |

## Pin-Belegung (siehe auch Verkabelung.pdf)

| Komponente | ESP32 Pin |
|------------|-----------|
| HC-SR04 Fach 1 | TRIG=26, ECHO=27 |
| HC-SR04 Fach 2 | TRIG=14, ECHO=12 |
| HC-SR04 Fach 3 | TRIG=33, ECHO=32 |
| HC-SR04 Fach 4 | TRIG=25, ECHO=35 |
| Relais Motor 1 | IN1=16 |
| Relais Motor 2 | IN2=17 |
| Relais Motor 3 | IN3=18 |
| Relais Motor 4 | IN4=19 |
| PN532 SDA | 21 |
| PN532 SCL | 22 |
| PN532 IRQ | 13 |
| PN532 RST | 15 |

## Troubleshooting

### ESP32 bootet in Endlosschleife
- Watchdog schlägt zu. Prüfe Serial-Output, Zeile vor Reset zeigt wo.
- Meist: fehlende Library oder falsche Pin-Nummer.

### Kein WLAN
- SSID/Passwort in config.h korrekt?
- X20CP1382 und ESP32 im gleichen Subnetz (Router-Subnet wie 192.168.2.x)?

### Keine TCP-Verbindung
- PLC-IP in config.h = tatsächliche IP der X20CP1382?
- PLC-Projekt auf X20CP1382 übertragen und läuft?
- Windows-Firewall blockiert Port 8000?

### HC-SR04 liest immer -1
- 5 V Versorgung am Sensor (nicht 3,3 V — HC-SR04 braucht 5 V)?
- TRIG/ECHO nicht verwechselt?

### PN532 nicht gefunden
- Modul im I2C-Modus konfiguriert (DIP-Schalter am Modul!)?
- SDA/SCL nicht verwechselt?
- 3,3 V an VCC (nicht 5 V)?
