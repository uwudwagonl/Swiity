# Swiity — VendingMachine (ESP32 Firmware)

## Repo
- GitHub: https://github.com/uwudwagonl/Swiity
- This project: `Swiity/Arduino/VendingMachine/` (PlatformIO project, sources in `src/`)
- ⚠️ **The AutomationStudio / PLC (AS) code in the repo is NOT up to date.** Do NOT use it as source of truth for pin mappings or signal contracts. The PLC side is owned by a second agent (branch `fix/active-low-relay`, PR #3).

## Architecture — ESP32 = pure sensor / NFC node
The ESP32 does **NOT** control motors and uses **no WiFi/TCP**. Motors are driven **exclusively by the B&R X20 SPS** (`doMotor[0..3]`). Motor code has been removed from the firmware.

The ESP32 signals the SPS over **5 discrete digital lines (1 bit each)**:
- GPIO16–19 → Fach-Status (HIGH = VOLL, LOW = LEER) from 4× HC-SR04
- GPIO23 → NFC erkannt (~2s HIGH pulse, any card, test only)

### Level shifting 3.3V → 24V via Elegoo 8-channel relay
The relay is used **only as a level-shifter** for the 5 ESP32 signals (IN1–IN5) — **NOT for motors, NOT as transistors**. (User has no BC547 and prefers the relay, which is already on hand.)

**Inversion chain (important):** An active-LOW relay inverts in the *same direction* as a BC547 NPN common-emitter. So the PLC's existing inversion code works unchanged AND the ESP32 firmware stays on **direct logic** (HIGH = VOLL). **Do NOT re-invert in firmware** — that would double-invert.

PLC-side inversion (for reference, owned by other agent):
```
gFach[i].istLeer = diEspFach[i] ? 1 : 0;
gNfc.erkannt     = diEspNfc ? 0 : 1;
```
IoMap: diEspFach[0..3] → X1.DigitalInput02–05, diEspNfc → X1.DigitalInput06.

## Pin map (from `src/config.h` — single source of truth)

### HC-SR04 ultrasonic (4×)
| Fach | TRIG | ECHO |
|------|------|------|
| 1 | GPIO26 | GPIO27 |
| 2 | GPIO14 | GPIO12 |
| 3 | GPIO33 | GPIO32 |
| 4 | GPIO25 | GPIO35 (input-only, OK for echo) |

Per sensor: **VCC → 5V**, GND → common GND.
⚠️ **ECHO is 5V** — use a voltage divider (e.g. 1k/2k) on each ECHO line down to 3.3V to protect ESP32 inputs. TRIG is fine direct (ESP32 drives 3.3V out).

### SPS bit lines (via relay)
| Signal | GPIO | Relay |
|--------|------|-------|
| Fach 1 status | 16 | IN1 |
| Fach 2 status | 17 | IN2 |
| Fach 3 status | 18 | IN3 |
| Fach 4 status | 19 | IN4 |
| NFC erkannt | 23 | IN5 |

Relay wiring (final, no changes planned ESP32-side):
- ESP32 GPIO → relay IN; VCC → 3.3V; JD-VCC → 5V (**jumper removed**); common GND
- Load side: 24V → COM, NO → X20 DI, 10k pulldown

### PN532 NFC (I2C)
| PN532 | ESP32 |
|-------|-------|
| SDA | GPIO21 |
| SCL | GPIO22 |
| IRQ | GPIO13 (optional) |
| RST | GPIO15 (optional) |
| VCC | 3.3V |
| GND | GND |

- Only **VCC, GND, SDA, SCL** are required for I2C. IRQ/RST can be left unconnected (lib polls over the bus; PN532 self-resets).
- Set the board's interface jumper/DIP to **I2C** (Elechouse V3: SET0=ON, SET1=OFF).
- GPIO22 (SCL) goes **only** to the PN532 — it is NOT one of the SPS bit lines.
- Constructor in `nfc_reader.cpp`: `new Adafruit_PN532(PN532_IRQ_PIN, PN532_RST_PIN, &Wire)`.

### Key constants (config.h)
`LEER_SCHWELLE_MM 50`, `HCSR04_TIMEOUT_US 30000`, `SENSOR_INTERVALL_MS 250`, `NFC_POLL_INTERVALL_MS 500`, `NFC_OUT_DAUER_MS 2000`, `SCHALL_MM_PRO_US 0.343`, `WATCHDOG_TIMEOUT_S 10`, `NVS_NAMESPACE "snackomat"`.

## Build / flash (PlatformIO)
`platformio.ini`:
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
upload_speed = 921600
lib_deps = adafruit/Adafruit PN532
```
- Board: ESP32-D0WD-V3 (esp32dev), Arduino framework.
- Build + flash: `pio run --target upload --upload-port <PORT>`
- Serial monitor: 115200 baud.

### USB / COM port (machine-specific — re-check on new machine!)
- ESP32 board uses a **CP2102 (Silicon Labs CP210x)** USB-UART bridge.
- On the original Win11 machine it was **COM4**. **Always set the port explicitly** — auto-detect can grab the wrong port (e.g. an Intel AMT SOL management port).
- If the port is missing / driver shows code 28: install Silicon Labs **CP210x Universal Windows Driver** (silabser.inf) via `pnputil /add-driver ... /install` (needs UAC). Driver download: https://www.silabs.com/documents/public/software/CP210x_Universal_Windows_Driver.zip
- Auto-reset works (CP2102 DTR/RTS) — no manual BOOT button needed. "Failed to connect: No serial data received" usually means **wrong port**, not a boot-button issue.
- On a new machine, install: VS Code + PlatformIO IDE extension (`platformio.platformio-ide`). PIO CLI lives at `<user>/.platformio/penv/Scripts/pio.exe`.

## Firmware source notes
- `src/config.h` — direct logic (HIGH=VOLL). No RELAY_ON/OFF defines, no motor defines.
- `src/fach_manager.cpp` — `digitalWrite(STATUS_PINS[i], leer ? LOW : HIGH)`.
- `src/nfc_reader.cpp` — any card → GPIO23 HIGH for 2s, then LOW.
- `src/preferences_manager.*` — motor-laufzeit methods removed (ESP32 is sensor-only).
- `src/hcsr04_sensor.*` — median-of-3 distance.

### Known issue (open)
In `nfc_reader.cpp::begin()`, `pinMode(NFC_OUT_PIN, OUTPUT)` runs **after** the PN532-not-found early return. So without a PN532 connected, GPIO23 (relay IN5) floats. Fix: move the GPIO23 init **before** the PN532 check so IN5 stays LOW even without a reader. (Not yet applied.)

## Current status
- Firmware builds (Flash ~23.7%, RAM ~6.7%) and was flashed successfully.
- Relay wired and confirmed final on the ESP32 side. Sensors/NFC not yet physically wired.
- Firmware changes uncommitted (motor removal + direct-logic revert).
