"""
Erzeugt die zwei Dokumentations-PDFs fuer das Snackomat-Projekt.
Ausfuehren mit: python _build_pdfs.py
"""
from pathlib import Path
from reportlab.lib.pagesizes import A4
from reportlab.platypus import (
    SimpleDocTemplate, Paragraph, Spacer, Table, TableStyle,
    PageBreak, ListFlowable, ListItem,
)
from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
from reportlab.lib import colors
from reportlab.lib.units import cm

OUT_DIR = Path(__file__).parent

# --- Styles -----------------------------------------------------------
styles = getSampleStyleSheet()
styles.add(ParagraphStyle(name="H1Ger", parent=styles["Heading1"],
           fontSize=18, textColor=colors.HexColor("#1a5276"), spaceBefore=12, spaceAfter=8))
styles.add(ParagraphStyle(name="H2Ger", parent=styles["Heading2"],
           fontSize=14, textColor=colors.HexColor("#2e86c1"), spaceBefore=10, spaceAfter=6))
styles.add(ParagraphStyle(name="H3Ger", parent=styles["Heading3"],
           fontSize=11, textColor=colors.HexColor("#154360"), spaceBefore=8, spaceAfter=4))
styles.add(ParagraphStyle(name="Body", parent=styles["BodyText"],
           fontSize=10, leading=13, spaceAfter=4))
styles.add(ParagraphStyle(name="Warn", parent=styles["BodyText"],
           fontSize=10, leading=13, backColor=colors.HexColor("#fdebd0"),
           borderColor=colors.HexColor("#ca6f1e"), borderWidth=1, borderPadding=6, spaceAfter=6))
styles.add(ParagraphStyle(name="Note", parent=styles["BodyText"],
           fontSize=10, leading=13, backColor=colors.HexColor("#eaf2f8"),
           borderColor=colors.HexColor("#2e86c1"), borderWidth=1, borderPadding=6, spaceAfter=6))
styles.add(ParagraphStyle(name="CodeBlk", parent=styles["Code"],
           fontName="Courier", fontSize=9, leading=11, backColor=colors.HexColor("#f4f6f6"),
           borderPadding=4, leftIndent=6, rightIndent=6))

def H1(t): return Paragraph(t, styles["H1Ger"])
def H2(t): return Paragraph(t, styles["H2Ger"])
def H3(t): return Paragraph(t, styles["H3Ger"])
def P(t):  return Paragraph(t, styles["Body"])
def N(t):  return Paragraph("<b>Hinweis:</b> " + t, styles["Note"])
def W(t):  return Paragraph("<b>Achtung:</b> " + t, styles["Warn"])
def C(t):  return Paragraph(t.replace("\n","<br/>"), styles["CodeBlk"])

def make_table(header, rows, col_widths=None):
    data = [header] + rows
    t = Table(data, colWidths=col_widths, repeatRows=1)
    t.setStyle(TableStyle([
        ("BACKGROUND", (0,0), (-1,0), colors.HexColor("#2e86c1")),
        ("TEXTCOLOR",  (0,0), (-1,0), colors.white),
        ("FONTNAME",   (0,0), (-1,0), "Helvetica-Bold"),
        ("FONTSIZE",   (0,0), (-1,-1), 9),
        ("BOTTOMPADDING",(0,0), (-1,0), 6),
        ("GRID", (0,0), (-1,-1), 0.5, colors.grey),
        ("ROWBACKGROUNDS", (0,1), (-1,-1),
            [colors.whitesmoke, colors.HexColor("#f4f6f6")]),
        ("VALIGN", (0,0), (-1,-1), "MIDDLE"),
    ]))
    return t

# ======================================================================
#  PDF 1: VERKABELUNG
# ======================================================================

def build_verkabelung_pdf():
    doc = SimpleDocTemplate(str(OUT_DIR / "01_Verkabelung.pdf"),
        pagesize=A4, leftMargin=1.8*cm, rightMargin=1.8*cm,
        topMargin=1.8*cm, bottomMargin=1.8*cm,
        title="Snackomat Verkabelung", author="Swiity Gruppe C")
    s = []

    s.append(H1("Snackomat - Verkabelung &amp; Inbetriebnahme"))
    s.append(P("Berufsschulprojekt Gruppe C (Swiity). "
               "Diese Anleitung fuehrt Schritt fuer Schritt durch den Aufbau."))

    s.append(H2("1. Uebersicht"))
    s.append(P("Der Automat besteht aus zwei intelligenten Einheiten:"))
    s.append(P("- <b>B&amp;R X20CP1382 + 6PPT50 Panel</b> (Touchscreen + SPS) - "
               "bedient die Visualisierung, zaehlt Muenzen und schaltet das Freigaberelais."))
    s.append(P("- <b>ESP32</b> - liest die 4 HC-SR04 Fuellstandssensoren, den "
               "PN532 NFC-Leser und steuert die 4 DC-Motoren ueber Relais."))
    s.append(P("Die beiden Einheiten kommunizieren <b>ueber LAN (TCP/IP, Port 8000)</b>. "
               "Der ESP32 verbindet sich als Client zur PLC-IP 192.168.2.20."))

    s.append(H2("2. Benoetigte Teile (Stueckliste)"))
    s.append(make_table(
        ["Pos", "Bauteil", "Menge", "Anmerkung"],
        [["1", "ESP32 DevKit (30 Pin)",             "1", "mit USB-C oder Micro-USB"],
         ["2", "HC-SR04 Ultraschall-Sensor",        "4", "5V Versorgung!"],
         ["3", "Relais-Modul 4-Kanal 5V",           "1", "active HIGH"],
         ["4", "DC-Getriebemotor 12V",              "4", "je Fach einer"],
         ["5", "PN532 NFC-Modul",                   "1", "im I2C-Modus"],
         ["6", "X20CP1382 CPU",                    "1", "bereits vorhanden"],
         ["7", "6PPT50.0502-10A Panel",            "1", "Touchscreen, VNC auf 5900"],
         ["8", "X20DI (X1) - Muenzeinwurf",        "1", "Lichtschranke DI01"],
         ["9", "X20DO (X3) - 4x Motor + LED",      "1", "DO01-DO04 Motoren, DO05 LED"],
         ["10", "X20AO2622 Motor-Speed (Analog)",  "1", "Geschwindigkeit 0-10V"],
         ["11", "Lichtschranke (1-Kanal)",           "1", "Muenzkanal"],
         ["12", "Netzteil 5V 3A",                   "1", "fuer ESP32+Relais+HC-SR04"],
         ["13", "Netzteil 12V 5A",                  "1", "fuer DC-Motoren"],
         ["14", "Reihenklemmen Weidmueller ZDK 2,5", "ca. 30", ""],
         ["15", "LAN-Kabel (Cat5 oder besser)",     "2", "1x X20CP1382, 1x ESP32 (via Router)"],
         ["16", "Ethernet-Router/Switch",           "1", "WLAN + LAN gemeinsam"],
         ["17", "Jumperkabel/Lappkabel div.",       "-", ""],
         ],
        col_widths=[1.2*cm, 6.5*cm, 1.5*cm, 6.8*cm]))

    s.append(PageBreak())
    s.append(H2("3. Pin-Belegung ESP32"))

    s.append(H3("3.1 HC-SR04 Ultraschallsensoren"))
    s.append(make_table(
        ["Fach", "TRIG (GPIO)", "ECHO (GPIO)", "VCC", "GND"],
        [["1", "26", "27", "5V", "GND"],
         ["2", "14", "12", "5V", "GND"],
         ["3", "33", "32", "5V", "GND"],
         ["4", "25", "35", "5V", "GND"]]))
    s.append(W("HC-SR04 braucht <b>5V</b> - an 3,3V antwortet er nicht oder falsch. "
               "ECHO liefert zwar 5V-Pegel, der ESP32 vertraegt das an den "
               "gewaehlten Input-Pins, aber sicherheitshalber einen 1k/2k "
               "Spannungsteiler einbauen, wenn du auf Nummer sicher gehen willst."))

    s.append(H3("3.2 Relais-Modul (Motorsteuerung)"))
    s.append(make_table(
        ["Relais", "Steuer-Pin (GPIO)", "Last (Motor an NO/COM)"],
        [["IN1 - Motor Fach 1", "16", "12V + Motor 1"],
         ["IN2 - Motor Fach 2", "17", "12V + Motor 2"],
         ["IN3 - Motor Fach 3", "18", "12V + Motor 3"],
         ["IN4 - Motor Fach 4", "19", "12V + Motor 4"]]))
    s.append(P("Versorgung Relais-Modul: <b>VCC = 5V</b> (ESP32-Netzteil), "
               "<b>GND = GND</b>. Zusaetzlich Jumper <b>JD-VCC</b> oeffnen "
               "und auf <b>12V</b> des Motor-Netzteils legen (Optokoppler-Trennung)."))
    s.append(W("5V und 12V GND muessen verbunden sein (gemeinsame Masse), "
               "sonst schaltet das Relais nicht. <b>Aber</b>: niemals 5V+ und 12V+ "
               "verbinden!"))

    s.append(H3("3.3 PN532 NFC-Modul (I2C)"))
    s.append(make_table(
        ["PN532 Pin", "ESP32 Pin", "Anmerkung"],
        [["VCC", "3V3", "NICHT 5V!"],
         ["GND", "GND", ""],
         ["SDA", "GPIO 21", "I2C Daten"],
         ["SCL", "GPIO 22", "I2C Takt"],
         ["IRQ", "GPIO 13", "Interrupt"],
         ["RST", "GPIO 15", "Reset"]]))
    s.append(W("Am PN532-Modul gibt es DIP-Schalter fuer den Modus. "
               "Fuer unser Setup muss <b>I2C</b> aktiv sein: "
               "SEL0 = AUS (OFF), SEL1 = EIN (ON)."))

    s.append(PageBreak())
    s.append(H2("4. Verdrahtung X20CP1382 &amp; X20-Module"))
    s.append(N("Die Hardware-Konfiguration kommt aus dem Schul-Basisprojekt "
               "und ist bereits im AS-Projekt angelegt (IoMap.iom). "
               "Die Klemmen-Bezeichnungen hier entsprechen dem AS-Projekt."))

    s.append(H3("4.1 Lichtschranke am Muenzkanal (Modul X1)"))
    s.append(make_table(
        ["Signal", "X1 Klemme", "AS-Variable"],
        [["+24V (Versorgung Lichtschranke)", "24V+", "-"],
         ["Ausgang Lichtschranke (Signal)", "DI 01", "diMuenzeinwurf"],
         ["GND", "GND", "-"]]))
    s.append(N("Die Muenz-Zaehlung erfolgt im Task <i>Program1</i> in "
               "<i>Logical/Program1/Cyclic.cpp</i>. Zaehler: "
               "<b>gCoin.summeCent</b> (jeder Impuls = <b>gCoin.wertProImpulsCent</b>, "
               "Default 10 Cent)."))

    s.append(H3("4.2 Motoren + LED am Digital-Output (Modul X3)"))
    s.append(make_table(
        ["Signal", "X3 Klemme", "AS-Variable"],
        [["Motor Fach 1", "DO 01", "doMotor[0]"],
         ["Motor Fach 2", "DO 02", "doMotor[1]"],
         ["Motor Fach 3", "DO 03", "doMotor[2]"],
         ["Motor Fach 4", "DO 04", "doMotor[3]"],
         ["LED-Beleuchtung", "DO 05", "doLedEin"]]))
    s.append(N("doMotor[] wird von der PLC als <b>Fallback</b> genutzt, falls "
               "ESP32 nicht verbunden ist. Im Normalbetrieb steuert der ESP32 "
               "die Motoren ueber seine eigenen Relais (siehe Abschnitt 3.2)."))

    s.append(H3("4.3 Motor-Speed Analog (Modul X20AO2622)"))
    s.append(P("Optional: Analog-Output fuer variable Motor-Geschwindigkeit. "
               "Variable <b>aoMotorspeed</b> (0-32767 = 0-10V)."))

    s.append(H3("4.4 Ethernet / Netzwerk"))
    s.append(P("Die X20CP1382 hat <b>IP 192.168.2.20</b> (siehe Cpu.pkg). "
               "ESP32 muss im gleichen Subnetz sein. Empfehlung: Router "
               "mit DHCP auf 192.168.2.x konfigurieren, ESP32 bekommt dann "
               "automatisch eine IP."))
    s.append(make_table(
        ["Geraet", "Anschluss", "Ziel"],
        [["X20CP1382 CPU", "ETH1", "Switch/Router"],
         ["6PPT50 Panel",  "ETH",  "gleicher Switch (VNC ueber Panel)"],
         ["ESP32",         "WLAN", "gleicher Router"],
         ["Laptop (Entwicklung)", "LAN/WLAN", "gleicher Router (fuer AS + ESP32-Upload)"]]))

    s.append(PageBreak())
    s.append(H2("5. Stromversorgung"))
    s.append(make_table(
        ["Verbraucher", "Spannung", "Max. Strom", "Quelle"],
        [["ESP32 + HC-SR04 + Relais-Spulen", "5V", "~1,5 A", "Netzteil 1 (5V 3A)"],
         ["PN532 NFC-Modul",                  "3,3V", "~0,1 A", "ESP32 3V3 Pin"],
         ["DC-Motoren (ueber Relais)",        "12V",  "je ~1 A", "Netzteil 2 (12V 5A)"],
         ["Lichtschranke",                     "24V", "0,05 A", "X20 Powersupply oder 24V NT"],
         ["X20CP1382 + 6PPT50 Panel",          "24V", "~1,5 A", "Panel-Netzteil"]]))
    s.append(W("<b>Alle Massen (GND) miteinander verbinden</b> - sonst gibt es "
               "unvorhersagbares Verhalten. Ausnahme: 24V-Bereich der PLC darf "
               "galvanisch getrennt bleiben, muss dann aber ueber Relais/Koppler "
               "laufen."))

    s.append(H2("6. Aufbau Schritt fuer Schritt"))
    steps = [
        "Alle Netzteile <b>ausschalten</b>.",
        "Reihenklemmen-Leiste montieren (Weidmueller ZDK 2,5).",
        "5V-Netzteil an ESP32 (Vin oder 5V Pin) anschliessen. GND nicht vergessen.",
        "HC-SR04-Sensoren 1-4 an die jeweiligen GPIO-Pins (siehe Tabelle 3.1) klemmen.",
        "Relais-Modul an 5V/GND + IN1-IN4. JD-VCC-Jumper abziehen, JD-VCC an 12V-Netzteil+.",
        "DC-Motoren an NO+COM der Relais-Kontakte. 12V+ an COM, Motor zwischen NO und 12V-.",
        "PN532 an 3V3/GND, SDA=21, SCL=22, IRQ=13, RST=15. DIP-Schalter I2C.",
        "Lichtschranke an X20 Modul X1 DI01 + 24V Versorgung.",
        "X20CP1382 + 6PPT50 Panel ueber Ethernet an Switch/Router. 24V-Netzteil an "
        "die oberen Klemmen (24V/GND).",
        "Netzteile nacheinander einschalten: zuerst 24V (PLC), dann 5V (ESP32), "
        "zuletzt 12V (Motoren).",
        "Panel bootet (AR G4.83). ESP32 bootet via USB-Power, wenn angesteckt. "
        "Im normalen Betrieb laeuft ESP32 ueber 5V-Netzteil.",
    ]
    s.append(ListFlowable(
        [ListItem(P(st)) for st in steps],
        bulletType="1", start="1"))

    s.append(PageBreak())
    s.append(H2("7. Erstes Hochfahren - Checkliste"))
    s.append(make_table(
        ["Nr", "Schritt", "Erwartung"],
        [["1", "ESP32 an Arduino IDE, Serial Monitor 115200", "Boot-Meldung 'Snackomat ESP32-Firmware'"],
         ["2", "config.h: WIFI_SSID und WIFI_PASSWORD anpassen",
                "Upload erfolgreich"],
         ["3", "Serial: WLAN verbinde... -> verbunden. IP: ...", "innerhalb 20 s"],
         ["4", "Serial: TCP verbinde zu 192.168.2.20:8000", "'TCP-Verbindung zur PLC steht'"],
         ["5", "PLC: Variable <b>gEsp.verbunden</b> via OPC-UA", "TRUE"],
         ["6", "Variable <b>Visu.page.actpage</b>", "0 (Init) oder 10 (Main)"],
         ["7", "Muenze einwerfen - <b>gCoin.summeCent</b>", "erhoeht sich um Wert pro Impuls (Default 10)"],
         ["8", "Karte an PN532 halten - <b>gNfc.uidHex</b>",
                "HEX-UID erscheint, gNfc.inWhitelist pruefen"]]))
    s.append(N("Zum Live-Testen per Claude MCP: sag einfach <i>'Lies die Variable "
               "Visu.page.actpage aus der PLC'</i>, dann wird per OPC-UA abgefragt."))

    s.append(H2("8. Troubleshooting"))
    s.append(make_table(
        ["Problem", "Ursache", "Fix"],
        [["ESP32 rebootet endlos", "Watchdog oder fehlende Lib", "Serial-Log pruefen"],
         ["HC-SR04 misst -1", "falsche Spannung (3,3V statt 5V)", "auf 5V umklemmen"],
         ["Relais klickt nicht", "GND nicht verbunden", "5V-GND und 12V-GND bruecken"],
         ["TCP verbindet nicht", "falsche IP oder PLC nicht aktiv",
                                  "in config.h PLC_IP pruefen, 'ping 192.168.2.20'"],
         ["PN532 nicht erkannt", "I2C DIP-Schalter falsch", "SEL0 OFF, SEL1 ON setzen"],
         ["Muenze zaehlt nicht", "Lichtschranke prellt",
                                  "Entprellzeit in Program1 Variables.var (entprellungMs) erhoehen"],
         ["Motor laeuft zu kurz/lang", "MOTOR_LAUFZEIT_DEFAULT_MS",
                                  "in config.h anpassen oder per OPC-UA Perm_motorLaufzeit[n] setzen"]]))

    doc.build(s)
    print("OK:", OUT_DIR / "01_Verkabelung.pdf")


# ======================================================================
#  PDF 2: VISU-ANLEITUNG (Klick-fuer-Klick)
# ======================================================================

def build_visu_pdf():
    doc = SimpleDocTemplate(str(OUT_DIR / "02_Visu_Anleitung.pdf"),
        pagesize=A4, leftMargin=1.8*cm, rightMargin=1.8*cm,
        topMargin=1.8*cm, bottomMargin=1.8*cm,
        title="Snackomat Visu-Anleitung", author="Swiity Gruppe C")
    s = []

    s.append(H1("Snackomat - Visualisierung in Automation Studio"))
    s.append(P("Diese Anleitung fuehrt Schritt fuer Schritt durch die Visu-Erstellung. "
               "Voraussetzung: Du hast das Projekt <b>Swiity_GruppeC.apj</b> in Automation "
               "Studio 4.12 geoeffnet und der Quellcode (Global.typ, Program/Cyclic.cpp etc.) "
               "ist vorhanden. Die Visu-Seiten wurden als Skelett angelegt - du musst sie jetzt "
               "in AS mit Widgets fuellen."))

    s.append(N("Sichere das Projekt vor der Bearbeitung (z.B. ganzen Ordner als ZIP kopieren). "
               "Die .page-Dateien sind XML - wenn etwas schief geht, kann eine Seite nicht "
               "mehr geoeffnet werden."))

    s.append(H2("1. Projekt bauen (Testlauf)"))
    s.append(P("Bevor du die Visu baust, stelle sicher dass die Logik kompiliert:"))
    s.append(ListFlowable([
        ListItem(P("AS oeffnen, Projekt Swiity_GruppeC.apj laden.")),
        ListItem(P("Im Projektbaum Rechtsklick auf Configuration <b>Config1</b> -> <i>Build Configuration</i>.")),
        ListItem(P("Output-Fenster beobachten - keine roten Fehler, nur ggf. Warnungen.")),
        ListItem(P("Bei 'Library AsTCP not found' -> siehe Abschnitt 2.1.")),
    ], bulletType="1"))

    s.append(H3("2.1 Fehlende Libraries hinzufuegen"))
    s.append(P("Im Projektbaum unter <b>Logical / Libraries</b>:"))
    s.append(ListFlowable([
        ListItem(P("Rechtsklick -> <i>Add Object -> Existing Library</i>.")),
        ListItem(P("Aus der Liste auswaehlen und je per Doppelklick hinzufuegen: "
                   "<b>AsTCP</b>, <b>AsString</b>, <b>AsBrStr</b>, <b>AsIOTime</b>.")),
        ListItem(P("Die jeweiligen Version koennen Defaults bleiben. Auf F7 (Build) pruefen.")),
    ], bulletType="1"))

    s.append(H2("2. Visu-Seiten-Uebersicht"))
    s.append(P("Im Projekt sind bereits folgende Seiten angelegt (Ordner "
               "<b>Logical/Visu/Pages/</b>):"))
    s.append(make_table(
        ["Datei", "Index", "Zweck", "Was du einbauen musst"],
        [["_00_Init.page", "0", "Start/Logo", "bereits fertig von Vorlage"],
         ["_10_Main.page", "10", "Produktauswahl (4 Fach-Buttons)", "Buttons + Fach-Anzeigen"],
         ["_20_BarBezahlen.page", "20", "Muenzzaehler + Bestaetigen", "Anzeige, Bestaetigen-Button"],
         ["_30_KarteBezahlen.page", "30", "Karte auflegen", "Symbol, Status, Bestaetigen"],
         ["_40_Ausgabe.page", "40", "Motor laeuft - Fortschritt", "Fortschrittsbalken"],
         ["_50_Danke.page", "50", "Dankeschoen", "Text, 'Zurueck'-Button"],
         ["_90_Fehler.page", "90", "Fehler/Leer", "Meldung, 'Zurueck'"],
         ["_900_Settings.page", "900", "Einstellungen", "von Vorlage - behalten"],
         ["_910_Settings1.page", "910", "Motor-Kalibrierung", "Buttons P910_Kalibriere1-4"]],
        col_widths=[4.2*cm, 1.0*cm, 5.0*cm, 6.0*cm]))

    s.append(PageBreak())
    s.append(H2("3. Schritt-fuer-Schritt: Seite _10_Main.page bauen"))
    s.append(P("Diese Seite ist das <b>Herz</b> der Bedienung. Sie zeigt 4 Produkte mit "
               "Preis/Bestand und ermoeglicht die Auswahl."))

    s.append(H3("3.1 Seite oeffnen"))
    s.append(P("Im Projektbaum unter <i>Logical/Visu/Pages/</i> Doppelklick auf "
               "<b>_10_Main.page</b>. Der Layout-Editor oeffnet sich."))

    s.append(H3("3.2 Vier Produkt-Buttons erstellen"))
    s.append(P("Wir brauchen 4 grosse Buttons, 2x2 Raster."))
    s.append(ListFlowable([
        ListItem(P("Toolbox (rechts) -> <b>Button</b> auswaehlen.")),
        ListItem(P("Rechteck aufziehen: <i>Position</i> links 50, oben 100, "
                   "Breite 320, Hoehe 150.")),
        ListItem(P("In den Properties (unten) <b>Name</b> setzen: "
                   "<b>btnFach1</b>.")),
        ListItem(P("Property <b>Text</b> -> '%s' und im Punkt <b>TextDataPoint</b> "
                   "binden auf: <i>::AsGlobalPV:gFach[0].name</i>.")),
        ListItem(P("Property <b>ActionDatapoint</b> -> "
                   "<i>::AsGlobalPV:Visu.buttons.P10_Selection1</i>. "
                   "Action-Type: <b>Pushbutton</b> "
                   "(setzt beim Klick auf 1 und die PLC loescht zurueck).")),
        ListItem(P("Button kopieren (Strg+C / Strg+V) und Kopien positionieren: "
                   "btnFach2 (rechts daneben, P10_Selection2), "
                   "btnFach3 (unten links, P10_Selection3), "
                   "btnFach4 (unten rechts, P10_Selection4).")),
    ], bulletType="1"))

    s.append(H3("3.3 Bestand/Preis unter jedem Button anzeigen"))
    s.append(ListFlowable([
        ListItem(P("Toolbox -> <b>Numeric Output</b>.")),
        ListItem(P("Neben btnFach1 platzieren (oder als Label darueber).")),
        ListItem(P("Property <b>DataPoint</b>: "
                   "<i>::AsGlobalPV:gFach[0].preisCent</i>. "
                   "<b>Format</b>: 'Preis: %3d Cent'.")),
        ListItem(P("Zweites Numeric Output darunter: DataPoint "
                   "<i>::AsGlobalPV:gFach[0].abstandCm</i>, Format 'Bestand: %2d cm'.")),
        ListItem(P("Drittes Feld: <b>Text Output</b> mit Status VOLL/LEER. "
                   "Property <b>Text</b>: '%s' Format, "
                   "<b>Variable</b>: <i>::AsGlobalPV:gFach[0].istLeer</i> "
                   "(BOOL -> Text wird zur Auswahl 'VOLL' / 'LEER' ueber Textgroup).")),
        ListItem(P("Alles fuer Fach 2-4 wiederholen (je Index 2, 3, 4).")),
    ], bulletType="1"))

    s.append(H3("3.4 Abbrechen-Button (oben rechts)"))
    s.append(P("Kleiner Button 'Abbrechen' mit ActionDatapoint "
               "<i>::AsGlobalPV:Visu.buttons.PXX_Abbrechen</i> - wird nur sichtbar, "
               "wenn <i>Visu.page.actpage &gt; 0</i> (Visibility-Expression)."))

    s.append(PageBreak())
    s.append(H2("4. Seite _20_BarBezahlen.page bauen"))
    s.append(ListFlowable([
        ListItem(P("Ueberschrift: <b>Text Output</b> 'Bitte Muenzen einwerfen'.")),
        ListItem(P("Gross in der Mitte: Numeric Output <b>gCoin.summeCent</b>, "
                   "Format 'Eingezahlt: %3d Cent'.")),
        ListItem(P("Darunter: Numeric Output <b>gSel.preisCent</b>, "
                   "Format 'Preis: %3d Cent'.")),
        ListItem(P("Fortschritt: <b>Progress Bar</b> EndValue = "
                   "<i>gSel.preisCent</i>, Value = <i>gCoin.summeCent</i>.")),
        ListItem(P("Button 'Bestaetigen': ActionDatapoint "
                   "<i>Visu.buttons.P20_BestaetigeBar</i>, "
                   "Enable-Expression: <i>gCoin.summeCent &gt;= gSel.preisCent</i>.")),
        ListItem(P("Button 'Abbrechen': ActionDatapoint "
                   "<i>Visu.buttons.PXX_Abbrechen</i>.")),
    ], bulletType="1"))

    s.append(H2("5. Seite _30_KarteBezahlen.page bauen (NFC)"))
    s.append(P("Das <b>Herzstueck</b> der Karten-Zahlung. Grosses Symbol, "
               "das auf die Stelle zeigt wo der PN532-Sensor hinter dem Display montiert ist."))
    s.append(ListFlowable([
        ListItem(P("Ueberschrift: 'Karte hier auflegen'")),
        ListItem(P("Gross in der Mitte: ein <b>Bitmap</b>-Control (Pfeil/Karten-Symbol) "
                   "das nach unten auf den NFC-Spot zeigt. Vorbereitete Bitmap unter "
                   "<i>Bitmaps/NfcSymbol.bminfo</i> anlegen (Import von "
                   "Vorlage/Internet).")),
        ListItem(P("Status-Textanzeige: <b>Text Output</b> mit "
                   "DataPoint <i>gNfc.uidHex</i>. Format: 'UID: %s'.")),
        ListItem(P("Ampel-Anzeige: Drei Texte/Bitmaps die je nach Bedingung sichtbar sind:"
                   "<br/>- 'Warte auf Karte...' wenn <i>gNfc.erkannt = 0</i>"
                   "<br/>- 'Karte akzeptiert' wenn <i>gNfc.erkannt=1 AND gNfc.inWhitelist=1</i>"
                   "<br/>- 'Karte unbekannt' wenn <i>gNfc.erkannt=1 AND gNfc.inWhitelist=0</i>")),
        ListItem(P("Button 'Bestaetigen': ActionDatapoint "
                   "<i>Visu.buttons.P30_BestaetigeKarte</i>, "
                   "Enable: <i>gNfc.inWhitelist = 1</i>.")),
        ListItem(P("Button 'Abbrechen': ActionDatapoint "
                   "<i>Visu.buttons.PXX_Abbrechen</i>.")),
    ], bulletType="1"))

    s.append(PageBreak())
    s.append(H2("6. Seite _40_Ausgabe, _50_Danke, _90_Fehler"))
    s.append(P("Einfach aufgebaut:"))
    s.append(ListFlowable([
        ListItem(P("_40: Text 'Bitte entnehmen Sie Ihr Produkt' + Progress Bar von 0..100.")),
        ListItem(P("_50: Text 'Danke!' + Auto-Weiter nach 3 s (erledigt die PLC).")),
        ListItem(P("_90: Roter Hintergrund, Text 'Fehler - bitte kontaktieren Sie Personal'.")),
    ], bulletType="1"))

    s.append(H2("7. Seite _910_Settings1 - Kalibrierung"))
    s.append(P("Auf der Settings-Seite 1 Kalibrier-Buttons fuer jedes Fach:"))
    s.append(ListFlowable([
        ListItem(P("Button 'Fach 1 kalibrieren' -> ActionDatapoint "
                   "<i>Visu.buttons.P910_Kalibriere1</i>.")),
        ListItem(P("Gleich fuer Fach 2, 3, 4.")),
        ListItem(P("Wenn gedrueckt: PLC sendet C:n an ESP32 -> ESP32 misst den aktuellen Abstand "
                   "und speichert ihn als 'VOLL'-Referenz in NVS.")),
    ], bulletType="1"))

    s.append(H2("8. Projekt bauen + Simulator testen"))
    s.append(ListFlowable([
        ListItem(P("<b>F7</b> oder Menue <i>Project -&gt; Build Configuration</i>.")),
        ListItem(P("Wenn 0 Fehler: <i>Configuration -&gt; Simulation starten</i> "
                   "(ARsim).")),
        ListItem(P("Im Simulator sollte die <b>_00_Init</b>-Seite 3 s sichtbar sein, "
                   "dann automatischer Wechsel auf <b>_10_Main</b>.")),
        ListItem(P("Tippe auf einen Fach-Button -> Wechsel auf _10 -> Bar/Karte-Auswahl. "
                   "Bar-Button -> _20. Du kannst mit dem 'Muenzeinwurf-Simulator' "
                   "<i>::AsGlobalPV:diMuenzeinwurf</i> auf TRUE/FALSE togglen.")),
    ], bulletType="1"))

    s.append(H2("9. Live-Test mit Claude MCP"))
    s.append(P("Mit dem installierten <i>br-automation-mcp</i> Server kannst du mich "
               "bitten:"))
    s.append(C('"Bau das Projekt und starte den Simulator"\n'
               '"Schreib 1 auf die Variable Visu.buttons.P10_Selection1"\n'
               '"Lies gFach[0].istLeer"\n'
               '"Setze gCoin.summeCent auf 200 und druecke P20_BestaetigeBar"'))
    s.append(P("So kannst du ganze Testsequenzen ohne Handkontakt zum Touchscreen durchspielen."))

    s.append(H2("10. Was noch? (optional)"))
    s.append(P("Wenn alles laeuft, kannst du die Visu noch verschoenern:"))
    s.append(ListFlowable([
        ListItem(P("Sprachumschaltung (Deutsch/Englisch) ueber <b>Visu.settings.Language</b> (schon vorbereitet in Vorlage).")),
        ListItem(P("Preise in der Settings-Seite editierbar: Numeric Input auf Perm_preisCent[n].")),
        ListItem(P("Alarm/Log-Seite fuer ESP32-Fehler - filtere ERR:-Nachrichten aus gEsp.letzteNachricht.")),
        ListItem(P("Screensaver nach 60 s Inaktivitaet.")),
    ], bulletType="1"))

    doc.build(s)
    print("OK:", OUT_DIR / "02_Visu_Anleitung.pdf")


if __name__ == "__main__":
    build_verkabelung_pdf()
    build_visu_pdf()
    print("Fertig - beide PDFs erstellt.")
