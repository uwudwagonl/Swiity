# Swiity – Vending Machine Project

**HTLVB 4AHWII Gruppe C**

A fully integrated vending machine project combining:

- **PLC / SPS control** – written in Structured Text (ST) with Automation Studio (B&R)
- **Arduino sensing** – sensor and actuator control via Arduino microcontrollers
- **Mechanical design** – CAD models, STL files, and 3D-printed parts
- **Electrical design** – wiring diagrams and schematics

---

## Repository Structure

```
Swiity/
├── AutomationStudio/          # PLC program (Structured Text, B&R Automation Studio)
│   └── VendingMachine/
│       ├── Main.st            # Main cyclic task
│       ├── StateMachine.st    # Vending state-machine logic
│       ├── Types.typ          # User-defined data types (UDTs)
│       └── Variables.var      # Global variable declarations
│
├── Arduino/                   # Arduino firmware for sensors & actuators
│   └── VendingMachine/
│       ├── VendingMachine.ino # Main Arduino sketch
│       ├── Sensors.h          # Sensor pin definitions and helper functions
│       └── Actuators.h        # Actuator pin definitions and helper functions
│
├── CAD/                       # Computer-aided design files
│   ├── Models/                # Native CAD source files (.step, .f3d, .sldprt, …)
│   └── Images/                # Rendered views and screenshots
│
├── 3DPrinting/                # 3D printing assets
│   ├── STL/                   # STL meshes ready for slicing
│   ├── GCode/                 # Sliced G-code files
│   └── PrintSettings/         # Slicer profiles / settings exports
│
├── Documentation/             # Project documentation
│   ├── Wiring/                # Wiring diagrams (PDF, PNG, Fritzing …)
│   ├── Schematics/            # Electrical schematics
│   └── BOM/                   # Bill of materials (BOM) spreadsheets
│
└── Images/                    # Project photos and renders
```

---

## Getting Started

### Automation Studio (PLC / SPS)

1. Open **B&R Automation Studio**.
2. Select *File → Open Project* and navigate to `AutomationStudio/VendingMachine/`.
3. Build the project (*Build → Build Solution*).
4. Transfer to the target controller via *Online → Transfer*.

### Arduino

1. Open `Arduino/VendingMachine/VendingMachine.ino` in the **Arduino IDE** (≥ 2.x).
2. Select the correct board and COM port.
3. Click *Upload*.

### 3D Printing

1. Find the desired part in `3DPrinting/STL/`.
2. Import the STL into your slicer (PrusaSlicer, Cura, …).
3. Load the matching profile from `3DPrinting/PrintSettings/` if available.
4. Slice, export G-code to `3DPrinting/GCode/`, and print.

---

## Requirements

| Component | Tool / Version |
|-----------|---------------|
| PLC code  | B&R Automation Studio 4.x, Structured Text (IEC 61131-3) |
| Arduino   | Arduino IDE 2.x, Arduino Uno / Mega (or compatible) |
| CAD       | Autodesk Fusion 360 / FreeCAD / SolidWorks |
| Slicing   | PrusaSlicer / Cura |

---

## Contributing

1. Create a feature branch: `git checkout -b feature/<your-feature>`
2. Commit your changes with descriptive messages.
3. Open a Pull Request against `main`.

---

## License

See [LICENSE](LICENSE) for details.
