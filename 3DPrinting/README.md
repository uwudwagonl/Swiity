# 3DPrinting

3D printing assets for vending machine parts.

## Sub-folders

| Folder | Contents |
|--------|----------|
| `STL/` | Mesh files ready for slicing (exported from `CAD/Models/`) |
| `GCode/` | Sliced G-code ready to send to the printer |
| `PrintSettings/` | Slicer profiles / settings exports |

## Recommended Print Settings (starting point)

| Parameter | Value |
|-----------|-------|
| Layer height | 0.2 mm |
| Infill | 20 % (structural parts: 40 %) |
| Perimeters | 3 |
| Supports | As needed |
| Material | PLA (enclosure) / PETG (load-bearing parts) |

## Naming Convention

STL files should mirror the CAD part number:

```
<PartNumber>_<Description>.stl
```

Example: `002_ProductSlotTray.stl`

## Workflow

1. Export `.stl` from the CAD model at the correct scale (mm, not inches).
2. Open in your slicer (PrusaSlicer / Cura).
3. Load the matching settings profile from `PrintSettings/`.
4. Slice and save the G-code to `GCode/` with the same part number prefix.
5. Transfer G-code to the printer (SD card / USB / OctoPrint).
