# Arduino

Arduino firmware that handles **physical sensor reading** and **actuator driving**
for the vending machine, acting as the I/O front-end to the PLC.

## Files

| File | Purpose |
|------|---------|
| `VendingMachine/VendingMachine.ino` | Main sketch – setup, loop, serial protocol |
| `VendingMachine/Sensors.h` | Sensor pin assignments and read helpers |
| `VendingMachine/Actuators.h` | Actuator pin assignments and control helpers |

## Wiring Summary

| Function | Arduino Pin | Notes |
|----------|-------------|-------|
| Coin sensor | D2 | Interrupt-capable |
| Product sensor | D3 | Interrupt-capable |
| Door sensor | D4 | Reed switch, INPUT_PULLUP |
| Temperature | A0 | NTC / DS18B20 |
| Stock sensors 0–7 | D5–D12 | IR reflection sensors |
| Motor enable | A1 | PWM output |
| Slot select bit 0 | A2 | 3-bit address LSB |
| Slot select bit 1 | A3 | 3-bit address |
| Slot select bit 2 | A4 | 3-bit address MSB |
| Change solenoid | A5 | Coin-return solenoid |
| Status LED | D13 | Built-in LED |
| Cooling fan | A6 | Relay output |

## Serial Protocol

Baud rate: **115 200**

### Arduino → PLC  (sensor report, sent every 100 ms)

```
S,<coin>,<product>,<door>,<temp>,<s0>,<s1>,<s2>,<s3>,<s4>,<s5>,<s6>,<s7>
```

- `coin`, `product`, `door`, `sN` – `0` or `1`
- `temp` – float (°C), one decimal place

Example: `S,0,0,0,6.4,1,1,1,0,1,1,1,1`

### PLC → Arduino  (actuator command)

```
A,<motorEn>,<slot>,<change>,<statusLED>,<fan>
```

- All fields – `0` or `1` (or slot index 0–7)

Example: `A,1,3,0,1,0`  → motor on, slot 3, change off, LED on, fan off

## How to Flash

1. Open `VendingMachine/VendingMachine.ino` in **Arduino IDE 2.x**.
2. Select the correct board (*Tools → Board*) and port (*Tools → Port*).
3. Click **Upload** (Ctrl + U).
