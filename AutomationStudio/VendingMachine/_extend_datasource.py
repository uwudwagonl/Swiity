"""
Erweitert DataSource.dso um die neuen Snackomat-Variablen,
damit sie im VC4-DataPoint-Browser sichtbar sind.
"""
from pathlib import Path

DSO = Path(__file__).parent / "Logical" / "VCShared" / "DataSources" / "DataSource.dso"

# ---- Helper: Format eines DataPoint-Eintrags ----
def dp(name, plc_type, vc_type=None, indent=8):
    if vc_type is None:
        vc_type = "INTEGER" if plc_type in ("USINT","UINT","INT","DINT","UDINT","SINT","REAL","WORD","BYTE","DWORD") else \
                  "BOOL" if plc_type == "BOOL" else \
                  "STRING"
    sp = " " * indent
    return f'''{sp}<DataPoint Name="{name}">
{sp}  <Property Name="ConnectedBySharedResource" Value="False"/>
{sp}  <Property Name="ConnectingVisus" Value="Visu"/>
{sp}  <Property Name="Description" Value=""/>
{sp}  <Property Name="PLCType" Value="{plc_type}"/>
{sp}  <Property Name="UpdateTime" Value="Default"/>
{sp}  <Property Name="UserID" Value="None"/>
{sp}  <Property Name="VCType" Value="{vc_type}"/>
{sp}</DataPoint>
'''

def folder_open(name, ft="Struct", indent=4):
    sp = " " * indent
    return f'''{sp}<Folder Name="{name}">
{sp}  <Property Name="Description" Value=""/>
{sp}  <Property Name="FolderType" Value="{ft}"/>
'''

def folder_close(indent=4):
    sp = " " * indent
    return f"{sp}</Folder>\n"

# ---- Neue DataPoints generieren ----
def build_new_entries():
    out = []

    # gFach[0..3] - Struktur tFach
    for i in range(4):
        out.append(folder_open(f"gFach[{i}]", "Struct", indent=4))
        out.append(dp("abstandCm", "UINT", indent=6))
        out.append(dp("istLeer", "BOOL", indent=6))
        out.append(dp("referenzCm", "UINT", indent=6))
        out.append(dp("preisCent", "UINT", indent=6))
        out.append(dp("name", "STRING", indent=6))
        out.append(folder_close(indent=4))

    # gSel - tAuswahl
    out.append(folder_open("gSel", "Struct"))
    out.append(dp("fachNr", "UINT", indent=6))
    out.append(dp("aktiv", "BOOL", indent=6))
    out.append(dp("preisCent", "UINT", indent=6))
    out.append(folder_close())

    # gPay - tZahlung
    out.append(folder_open("gPay", "Struct"))
    out.append(dp("modus", "UINT", indent=6))
    out.append(dp("eingezahltCent", "UINT", indent=6))
    out.append(dp("status", "UINT", indent=6))
    out.append(dp("startZeitMs", "UDINT", indent=6))
    out.append(folder_close())

    # gNfc - tNfcInfo
    out.append(folder_open("gNfc", "Struct"))
    out.append(dp("uidHex", "STRING", indent=6))
    out.append(dp("zeitstempelMs", "UDINT", indent=6))
    out.append(dp("erkannt", "BOOL", indent=6))
    out.append(dp("inWhitelist", "BOOL", indent=6))
    out.append(folder_close())

    # gCoin - tMuenze
    out.append(folder_open("gCoin", "Struct"))
    out.append(dp("impulse", "UDINT", indent=6))
    out.append(dp("summeCent", "UINT", indent=6))
    out.append(dp("wertProImpulsCent", "UINT", indent=6))
    out.append(dp("letzterImpulsMs", "UDINT", indent=6))
    out.append(folder_close())

    # gEsp - tEspStatus
    out.append(folder_open("gEsp", "Struct"))
    out.append(dp("verbunden", "BOOL", indent=6))
    out.append(dp("letzterPingMs", "UDINT", indent=6))
    out.append(dp("letzteNachricht", "STRING", indent=6))
    out.append(dp("anzahlFehler", "UINT", indent=6))
    out.append(folder_close())

    return "".join(out)


# ---- DataSource.dso lesen, erweitern, schreiben ----
text = DSO.read_text(encoding="utf-8")

# Bestehende neue Eintraege koennten schon da sein -> nur einfuegen wenn nicht
if "Name=\"gFach[0]\"" in text:
    print("gFach bereits registriert -> Skript schon mal gelaufen, nichts zu tun.")
else:
    # Vor </DataPoints> einfuegen
    new_entries = build_new_entries()
    text = text.replace("  </DataPoints>", new_entries + "  </DataPoints>")
    DSO.write_text(text, encoding="utf-8")
    print(f"OK: {len(new_entries)} Zeichen neuer DataPoint-XML eingefuegt.")

# Auch noch die fehlenden neuen Visu.buttons registrieren falls nicht da
NEW_BUTTONS = [
    "P10_WaehleBar", "P10_WaehleKarte",
    "P20_BestaetigeBar", "P30_BestaetigeKarte",
    "PXX_Abbrechen",
    "P910_Kalibriere1", "P910_Kalibriere2",
    "P910_Kalibriere3", "P910_Kalibriere4",
]
text = DSO.read_text(encoding="utf-8")
button_xml = "".join(dp(b, "USINT") for b in NEW_BUTTONS if f'Name="{b}"' not in text)
if button_xml:
    # Vor dem ersten </Folder> nach "Folder Name=\"buttons\"" einfuegen
    marker = '<DataPoint Name="P910_MoveMotor">'
    end_marker = '</DataPoint>\n      </Folder>\n      <Folder Name="settings">'
    if end_marker in text:
        text = text.replace(end_marker, end_marker.split('\n      </Folder>')[0] + '\n      </Folder>\n      <Folder Name="settings">'.replace('\n      </Folder>', '\n' + button_xml + '      </Folder>'))
    DSO.write_text(text, encoding="utf-8")
    print(f"OK: Neue Buttons registriert: {[b for b in NEW_BUTTONS if f'Name=' + chr(34) + b + chr(34) not in text]}")

print("Fertig.")
