# CAD

Computer-Aided Design files for the vending machine enclosure and mechanical parts.

## Sub-folders

| Folder | Contents |
|--------|----------|
| `Models/` | Native CAD source files (`.f3d`, `.step`, `.iges`, `.sldprt`, …) |
| `Images/` | Rendered views, screenshots, and exploded diagrams |

## Recommended Naming Convention

```
<PartNumber>_<Description>_v<Version>.<ext>
```

Examples:
- `001_OuterEnclosure_v1.step`
- `002_ProductSlotTray_v2.f3d`
- `003_CoinChute_v1.step`

## Software

- **Autodesk Fusion 360** (`.f3d`) – preferred for collaborative editing
- **FreeCAD** (`.FCStd`) – open-source alternative
- **SolidWorks** (`.sldprt` / `.sldasm`) – if available at school

Always export a neutral `.step` file alongside the native format so any team
member can open the geometry regardless of which CAD tool they use.
