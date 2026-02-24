# Documentation

Project documentation for the vending machine.

## Sub-folders

| Folder | Contents |
|--------|----------|
| `Wiring/` | Wiring diagrams (Fritzing `.fzz`, PDF exports, PNG images) |
| `Schematics/` | Electrical schematics (KiCad, Eagle, or PDF) |
| `BOM/` | Bill of Materials spreadsheets (`.csv` / `.xlsx`) |

## Document Naming Convention

```
<DocType>_<Description>_v<Version>.<ext>
```

Examples:
- `Wiring_ArduinoToSensors_v1.pdf`
- `Schematic_PowerSupply_v2.pdf`
- `BOM_ElectricalComponents_v1.csv`

## BOM Template

A BOM CSV should contain at minimum:

| Column | Description |
|--------|-------------|
| `ItemNo` | Sequential item number |
| `Quantity` | Number of units required |
| `Description` | Human-readable description |
| `Manufacturer` | Manufacturer name |
| `PartNumber` | Manufacturer part number |
| `Supplier` | Where to order (e.g., DigiKey, Mouser, Conrad) |
| `SupplierPN` | Supplier part number |
| `UnitPrice` | Price per unit (€) |
| `Notes` | Any additional notes |
