# AutomationStudio

Structured Text (ST) source code for the vending machine PLC / SPS controller,
developed with **B&R Automation Studio** (IEC 61131-3).

## Files

| File | Purpose |
|------|---------|
| `Main.st` | Main cyclic program – entry point called every PLC scan |
| `StateMachine.st` | Vending state-machine function block (idle → coin → select → dispense → collect → change) |
| `Types.typ` | User-defined data types (enums, structs) |
| `Variables.var` | Global variable and constant declarations |

## State Machine Overview

```
IDLE ──► COIN_CHECK ──► SELECT ──► DISPENSE ──► COLLECT ──► CHANGE ──► IDLE
  ▲                                                                       │
  └────────────────────────────── (no change due) ───────────────────────┘

Any state ──► MAINTENANCE  (when service door opens)
Any state ──► ERROR        (on fault condition)
ERROR     ──► IDLE         (after 5 s auto-reset)
```

## How to Open in Automation Studio

1. Launch **B&R Automation Studio 4.x**.
2. *File → Open Project* → select this folder.
3. Assign the correct hardware configuration for your PLC target.
4. *Build → Build Solution* to compile.
5. *Online → Transfer* to deploy to the controller.

## I/O Mapping

Sensors and actuators are exchanged with the Arduino layer via digital/analog
I/O or a serial interface. Map the `gSensors` and `gActuators` global variables
to the corresponding hardware channels in the Automation Studio I/O mapping editor.
