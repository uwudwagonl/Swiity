# -*- coding: utf-8 -*-
"""
Generator fuer die anfaengerfreundliche Verkabelungs-Anleitung
Relaismodul -> B&R X20 SPS.  Erzeugt Verkabelung_Relais_zu_SPS_v1.pdf
"""
from reportlab.lib.pagesizes import A4
from reportlab.lib.units import mm
from reportlab.lib import colors
from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
from reportlab.lib.enums import TA_LEFT, TA_CENTER
from reportlab.platypus import (
    BaseDocTemplate, PageTemplate, Frame, Paragraph, Spacer, Table, TableStyle,
    ListFlowable, ListItem, Flowable, KeepTogether, PageBreak, HRFlowable
)

# ---------- Farben ----------
BLUE   = colors.HexColor("#1f4e79")
LBLUE  = colors.HexColor("#dce6f1")
ORANGE = colors.HexColor("#c55a11")
LORNG  = colors.HexColor("#fbe4d5")
GREEN  = colors.HexColor("#548235")
LGREEN = colors.HexColor("#e2efda")
RED    = colors.HexColor("#c00000")
LRED   = colors.HexColor("#fbdada")
GOLD   = colors.HexColor("#bf9000")
GREY   = colors.HexColor("#595959")
LGREY  = colors.HexColor("#f2f2f2")

OUT = "Verkabelung_Relais_zu_SPS.pdf"

# ---------- Styles ----------
styles = getSampleStyleSheet()
S = {}
S["title"] = ParagraphStyle("title", parent=styles["Title"], fontName="Helvetica-Bold",
                            fontSize=26, textColor=BLUE, spaceAfter=4, leading=30)
S["sub"]   = ParagraphStyle("sub", parent=styles["Normal"], fontSize=12, textColor=GREY,
                            alignment=TA_CENTER, spaceAfter=2)
S["h1"]    = ParagraphStyle("h1", parent=styles["Heading1"], fontName="Helvetica-Bold",
                            fontSize=16, textColor=colors.white, backColor=BLUE,
                            borderPadding=(6,8,6,8), spaceBefore=14, spaceAfter=10, leading=20)
S["h2"]    = ParagraphStyle("h2", parent=styles["Heading2"], fontName="Helvetica-Bold",
                            fontSize=13, textColor=BLUE, spaceBefore=10, spaceAfter=5, leading=16)
S["body"]  = ParagraphStyle("body", parent=styles["Normal"], fontSize=10.5, leading=15,
                            spaceAfter=6, alignment=TA_LEFT)
S["small"] = ParagraphStyle("small", parent=styles["Normal"], fontSize=9, leading=12,
                            textColor=GREY)
S["cell"]  = ParagraphStyle("cell", parent=styles["Normal"], fontSize=9.5, leading=12)
S["cellb"] = ParagraphStyle("cellb", parent=styles["Normal"], fontSize=9.5, leading=12,
                            fontName="Helvetica-Bold")
S["cellc"] = ParagraphStyle("cellc", parent=S["cell"], alignment=TA_CENTER)
S["cellcb"]= ParagraphStyle("cellcb", parent=S["cellb"], alignment=TA_CENTER)
S["white"] = ParagraphStyle("white", parent=S["cellb"], textColor=colors.white)
S["mono"]  = ParagraphStyle("mono", parent=styles["Normal"], fontName="Courier-Bold", fontSize=10)
S["step"]  = ParagraphStyle("step", parent=S["body"], fontSize=11, leading=16, spaceAfter=8)

def P(t, s="body"):  return Paragraph(t, S[s])

# ---------- Callout-Box ----------
def box(title, text, kind="info"):
    cmap = {"info":(LBLUE,BLUE,"i  INFO"),
            "warn":(LORNG,ORANGE,"!  ACHTUNG"),
            "danger":(LRED,RED,"X  GEFAHR"),
            "ok":(LGREEN,GREEN,"OK  TIPP")}
    bg, br, lbl = cmap[kind]
    head = Paragraph(f'<b>{title}</b>', ParagraphStyle("bh", parent=S["body"],
                     fontName="Helvetica-Bold", textColor=br, fontSize=11, spaceAfter=3))
    bodyp = Paragraph(text, ParagraphStyle("bb", parent=S["body"], spaceAfter=0))
    t = Table([[head],[bodyp]], colWidths=[150*mm])
    t.setStyle(TableStyle([
        ("BACKGROUND",(0,0),(-1,-1), bg),
        ("BOX",(0,0),(-1,-1), 1.2, br),
        ("LINEBEFORE",(0,0),(0,-1), 4, br),
        ("LEFTPADDING",(0,0),(-1,-1), 10),
        ("RIGHTPADDING",(0,0),(-1,-1), 10),
        ("TOPPADDING",(0,0),(-1,-1), 7),
        ("BOTTOMPADDING",(0,0),(-1,-1), 7),
    ]))
    return t

# ---------- Diagramm: ein Kanal ----------
class ChannelDiagram(Flowable):
    """Zeichnet eine Strecke: 24V -> COM -> Relais -> NO -> X20 DI (ohne Widerstand)."""
    def __init__(self, width=150*mm, height=50*mm):
        self.width = width; self.height = height
    def wrap(self, *a): return (self.width, self.height)
    def draw(self):
        c = self.canv
        c.setFont("Helvetica-Bold", 9)
        # Relais-Kasten
        rx, ry, rw, rh = 58*mm, 18*mm, 34*mm, 24*mm
        c.setFillColor(LBLUE); c.setStrokeColor(BLUE); c.setLineWidth(1.2)
        c.roundRect(rx, ry, rw, rh, 4, fill=1, stroke=1)
        c.setFillColor(BLUE)
        c.drawCentredString(rx+rw/2, ry+rh-11, "RELAIS-KANAL")
        c.setFont("Helvetica", 8)
        c.drawCentredString(rx+rw/2, ry+rh-21, "(Kontakt oben)")
        # COM Punkt (links am Relais)
        comx, comy = rx, ry+rh/2
        nox,  noy  = rx+rw, ry+rh/2
        # 24V Klemme links
        c.setFillColor(RED); c.setStrokeColor(RED); c.setLineWidth(1.6)
        c.circle(10*mm, comy, 2, fill=1)
        c.setFont("Helvetica-Bold", 9); c.setFillColor(RED)
        c.drawString(4*mm, comy+4*mm, "SPS +24V")
        c.line(10*mm, comy, comx, comy)
        c.setFillColor(BLUE); c.setFont("Helvetica-Bold",8)
        c.drawString(comx-9*mm, comy-9, "COM")
        # NO -> X20 DI rechts (direkt, kein Widerstand)
        c.setStrokeColor(GREEN); c.setFillColor(GREEN); c.setLineWidth(1.6)
        c.drawString(nox+1*mm, noy+2, "NO")
        c.line(nox, noy, 118*mm, noy)
        # X20 DI Klemme
        c.setFillColor(BLUE); c.setStrokeColor(BLUE)
        c.roundRect(118*mm, noy-5*mm, 28*mm, 10*mm, 2, fill=1, stroke=1)
        c.setFillColor(colors.white); c.setFont("Helvetica-Bold",8)
        c.drawCentredString(132*mm, noy-1, "X2  DI-Eingang")
        # IN-Pin (ESP32-Seite) unten am Relais
        c.setStrokeColor(ORANGE); c.setLineWidth(1.2); c.setDash(3,2)
        c.line(rx+rw/2, ry, rx+rw/2, 4*mm)
        c.setDash()
        c.setFillColor(ORANGE); c.setFont("Helvetica-Oblique",8)
        c.drawCentredString(rx+rw/2, 1*mm, "IN <- ESP32 (Freund)")

# ---------- Diagramm: Relaismodul Ober/Unterseite ----------
class ModuleSides(Flowable):
    def __init__(self, width=150*mm, height=58*mm):
        self.width=width; self.height=height
    def wrap(self,*a): return (self.width,self.height)
    def draw(self):
        c=self.canv
        # Platine
        c.setFillColor(colors.HexColor("#2f5597")); c.setStrokeColor(colors.black)
        c.roundRect(20*mm, 14*mm, 110*mm, 30*mm, 3, fill=1, stroke=1)
        # obere Klemmen
        c.setFillColor(colors.HexColor("#3a6ea5"))
        for i in range(8):
            x=24*mm+i*13*mm
            c.setFillColor(GOLD)
            c.rect(x, 40*mm, 10*mm, 8*mm, fill=1, stroke=1)
        c.setFillColor(GREEN); c.setFont("Helvetica-Bold",10)
        c.drawString(20*mm, 51*mm, "OBEN: Schraubklemmen  NO / COM / NC")
        c.setFillColor(GREEN); c.setFont("Helvetica-Bold",9)
        c.drawString(132*mm, 43*mm, "= DEINE")
        c.drawString(132*mm, 39*mm, "Seite (SPS)")
        # untere Stiftleiste
        c.setFillColor(colors.black)
        for i in range(10):
            x=30*mm+i*9*mm
            c.rect(x, 9*mm, 5*mm, 5*mm, fill=1, stroke=1)
        c.setFillColor(ORANGE); c.setFont("Helvetica-Bold",10)
        c.drawString(20*mm, 3*mm, "UNTEN: GND  IN1..IN8  VCC")
        c.setFillColor(ORANGE); c.setFont("Helvetica-Bold",9)
        c.drawString(132*mm, 11*mm, "= FREUND")
        c.drawString(132*mm, 7*mm, "(ESP32)")

# ---------- Tabellen-Helfer ----------
def header_row(cells):
    return [Paragraph(x, S["white"]) for x in cells]

def styled_table(data, colWidths, header_bg=BLUE, zebra=True):
    t = Table(data, colWidths=colWidths, repeatRows=1)
    cmds = [
        ("BACKGROUND",(0,0),(-1,0), header_bg),
        ("GRID",(0,0),(-1,-1), 0.5, colors.HexColor("#bfbfbf")),
        ("VALIGN",(0,0),(-1,-1),"MIDDLE"),
        ("TOPPADDING",(0,0),(-1,-1),5),
        ("BOTTOMPADDING",(0,0),(-1,-1),5),
        ("LEFTPADDING",(0,0),(-1,-1),6),
        ("RIGHTPADDING",(0,0),(-1,-1),6),
    ]
    if zebra:
        for r in range(1,len(data)):
            if r%2==0: cmds.append(("BACKGROUND",(0,r),(-1,r), LGREY))
    t.setStyle(TableStyle(cmds))
    return t

# ====================================================================
# DOKUMENT
# ====================================================================
story = []

def hr(): return HRFlowable(width="100%", thickness=1, color=colors.HexColor("#d0d0d0"),
                            spaceBefore=4, spaceAfter=8)

# ---- Titelseite ----
story.append(Spacer(1, 30*mm))
story.append(P("Verkabelung", "title"))
story.append(P("Relaismodul &rarr; B&amp;R X20 SPS", "title"))
story.append(Spacer(1, 4*mm))
story.append(P("Schritt-fuer-Schritt Anleitung fuer Anfaenger", "sub"))
story.append(P("Snackautomat &bdquo;Swiity&ldquo; &middot; HTL Voecklabruck &middot; SPS-Seite", "sub"))
story.append(Spacer(1, 10*mm))
story.append(box("Worum geht es hier?",
    "Der ESP32 (Sensor-Computer) muss der SPS melden, welches Fach voll/leer ist und ob eine "
    "NFC-Karte erkannt wurde. Der ESP32 spricht aber nur mit <b>3,3 Volt</b>, die SPS-Eingaenge "
    "brauchen <b>24 Volt</b>. Das <b>Relaismodul</b> uebersetzt zwischen diesen Spannungen. "
    "<b>Deine Aufgabe:</b> die Seite vom Relaismodul zur SPS verkabeln.", "info"))
story.append(Spacer(1, 6*mm))
story.append(box("Die 3 wichtigsten Regeln vorab",
    "1) <b>Strom AUS</b> beim Verkabeln.<br/>"
    "2) Am Relais nur <b>NO</b> und <b>COM</b> benutzen &ndash; <b>NC bleibt frei</b>.<br/>"
    "3) <b>Keine Widerstaende</b> &ndash; NO geht direkt auf den SPS-Eingang (so will es der Lehrer).", "ok"))
story.append(PageBreak())

# ---- 1. Das grosse Bild ----
story.append(P("1. Das grosse Bild", "h1"))
story.append(P("Es gibt zwei &bdquo;Gehirne&ldquo; im Automaten, die sich Infos zuschicken:", "body"))
story.append(styled_table([
    header_row(["Geraet", "Was es macht", "Wer kuemmert sich"]),
    [P("<b>ESP32</b>", "cellb"), P("Liest die 4 Fuellstand-Sensoren und den NFC-Leser. Gibt Signale mit 3,3V aus.", "cell"), P("Dein Freund", "cellc")],
    [P("<b>Relaismodul</b>", "cellb"), P("Wandelt 3,3V (ESP32) in 24V (SPS) um. 8 Kanaele, wir brauchen 5.", "cell"), P("ESP32 unten / DU oben", "cellc")],
    [P("<b>X20 SPS</b>", "cellb"), P("Steuert alles, zeigt Visu, schaltet Motoren. Liest die 5 Signale als 24V-Eingaenge.", "cell"), P("DU", "cellc")],
], [32*mm, 86*mm, 32*mm]))
story.append(Spacer(1, 5*mm))
story.append(P("Der Signalweg von einem Fach-Sensor bis zur SPS:", "body"))
story.append(P("ESP32-Pin (3,3V) &rarr; Relais IN (unten) &rarr; Relais-Kontakt (oben) &rarr; 24V &rarr; SPS-Eingang", "mono"))
story.append(Spacer(1, 4*mm))
story.append(box("Warum ein Relais und kein Kabel direkt?",
    "Hielte man die 3,3V des ESP32 direkt an den SPS-Eingang, wuerde die SPS das als &bdquo;AUS&ldquo; "
    "lesen &ndash; sie braucht ~24V fuer ein &bdquo;EIN&ldquo;. Das Relais ist ein elektrischer Schalter: "
    "das schwache ESP32-Signal schaltet den Kontakt, und ueber den Kontakt legen wir die vollen 24V "
    "der SPS auf den Eingang. Bonus: beide Stromkreise sind galvanisch getrennt (Schutz).", "info"))

# ---- 2. Das Relaismodul verstehen ----
story.append(P("2. Das Relaismodul verstehen: oben vs. unten", "h1"))
story.append(P("Das Modul hat zwei Seiten. Verwechsle sie nicht!", "body"))
story.append(ModuleSides())
story.append(Spacer(1, 3*mm))
story.append(styled_table([
    header_row(["Seite", "Anschluesse", "Wer", "Spannung"]),
    [P("<b>UNTEN</b><br/>Stiftleiste", "cellb"), P("GND, IN1&ndash;IN8, VCC, JD-VCC", "cell"), P("Freund (ESP32)", "cellc"), P("3,3V / 5V", "cellc")],
    [P("<b>OBEN</b><br/>Schraubklemmen", "cellb"), P("pro Kanal: <b>NO</b> / <b>COM</b> / NC", "cell"), P("<b>DU (SPS)</b>", "cellcb"), P("24V", "cellc")],
], [30*mm, 60*mm, 35*mm, 25*mm]))
story.append(Spacer(1, 4*mm))
story.append(box("Was bedeuten NO, COM, NC?",
    "Jeder Relaiskanal hat oben 3 Schrauben (ein Umschalter):<br/>"
    "&bull; <b>COM</b> = gemeinsamer Anschluss (hier kommt +24V drauf)<br/>"
    "&bull; <b>NO</b> = &bdquo;normally open&ldquo;, offen wenn Relais aus &ndash; <b>das benutzen wir</b><br/>"
    "&bull; <b>NC</b> = &bdquo;normally closed&ldquo;, geschlossen wenn Relais aus &ndash; <b>frei lassen!</b><br/>"
    "Schaltet das Relais, verbindet es COM mit NO &ndash; und die 24V gelangen zur SPS.", "ok"))
story.append(box("NICHT NC verwenden",
    "Wenn du aus Versehen NC statt NO nimmst, ist das Signal genau verkehrt herum: Die SPS denkt, "
    "ein volles Fach ist leer und umgekehrt. Immer <b>NO</b>!", "warn"))
story.append(PageBreak())

# ---- 3. Deine Aufgabe ----
story.append(P("3. Deine Aufgabe in einem Satz", "h1"))
story.append(box("Kurzfassung",
    "Hole <b>+24V</b> von Klemme <b>2.8</b> (Steckplatz X2, rechts unten) auf alle 5 <b>COM</b>-Klemmen. "
    "Verbinde jeden <b>NO</b>-Kontakt direkt mit einem digitalen Eingang auf <b>Steckplatz X2</b> "
    "(DI 2 bis DI 6). <b>Keine Widerstaende</b> &ndash; fertig.", "ok"))
story.append(Spacer(1, 3*mm))
story.append(P("So sieht <b>ein</b> Kanal aus (das machst du 5x):", "h2"))
story.append(ChannelDiagram())

# ---- 4. Belegungstabelle ----
story.append(P("4. Welcher Kanal auf welche SPS-Klemme?", "h1"))
story.append(P("Wir benutzen <b>Steckplatz X2</b> der CPU (X20CP1382). X2 hat 14 digitale Eingaenge "
               "und unten eine eigene <b>+24-V-Geber-Klemme</b>. Die Klemmen sind in 2 Spalten "
               "(links = 1.x, rechts = 2.x) und 8 Reihen angeordnet:", "body"))
story.append(styled_table([
    header_row(["Relais-Kanal", "NO &rarr; X2-Klemme", "Eingang", "SPS-Variable", "Bedeutung"]),
    [P("&ndash;", "cellc"), P("<b>1.1</b> (links, Reihe 1)", "cellc"), P("DI 1", "cellc"), P("diMuenzeinwurf", "cell"), P("Muenze (spaeter)", "cell")],
    [P("CH1", "cellcb"), P("<b>2.1</b> (rechts, Reihe 1)", "cellc"), P("DI 2", "cellc"), P("diEspFach[0]", "cell"), P("Fach 1", "cell")],
    [P("CH2", "cellcb"), P("<b>1.2</b> (links, Reihe 2)", "cellc"), P("DI 3", "cellc"), P("diEspFach[1]", "cell"), P("Fach 2", "cell")],
    [P("CH3", "cellcb"), P("<b>2.2</b> (rechts, Reihe 2)", "cellc"), P("DI 4", "cellc"), P("diEspFach[2]", "cell"), P("Fach 3", "cell")],
    [P("CH4", "cellcb"), P("<b>1.3</b> (links, Reihe 3)", "cellc"), P("DI 5", "cellc"), P("diEspFach[3]", "cell"), P("Fach 4", "cell")],
    [P("CH5", "cellcb"), P("<b>2.3</b> (rechts, Reihe 3)", "cellc"), P("DI 6", "cellc"), P("diEspNfc", "cell"), P("NFC erkannt", "cell")],
], [22*mm, 38*mm, 18*mm, 30*mm, 28*mm]))
story.append(Spacer(1, 3*mm))
story.append(box("Und das +24V?",
    "Die +24V fuer alle COM holst du von Klemme <b>2.8</b> (Steckplatz X2, <b>rechts ganz unten</b>, "
    "beschriftet &bdquo;+24 V Geber&ldquo;). Klemme <b>1.8</b> daneben (links unten) ist <b>GND/0V</b> &ndash; "
    "die brauchst du fuer das Relais NICHT (siehe Kapitel 5).", "info"))
story.append(box("Wo ist X2 in der Hardware?",
    "X2 ist der <b>mittlere</b> der drei integrierten I/O-Steckplaetze, die direkt rechts an der CPU "
    "X20CP1382 sitzen (X1 | X2 | X3). NICHT das olivgruene Modul ganz rechts (das ist der "
    "Analog-Ausgang X20AO2622 mit den rot/blauen Kabeln).", "ok"))
story.append(box("Zur Sicherheit in Automation Studio pruefen",
    "Physical View &rarr; Doppelklick auf <b>X20CP1382</b> &rarr; &bdquo;Anschlussbelegung Steckplatz X2&ldquo;. "
    "Dort siehst du DI 1&hellip;DI 14 sowie &bdquo;+24 V Geber&ldquo; und &bdquo;GND&ldquo;. So bist du 100% "
    "sicher, welche Schraube welcher Eingang ist, bevor du 24V anlegst.", "ok"))
story.append(PageBreak())

# ---- 5. Sammelleitungen ----
story.append(P("5. Die Sammelleitungen (+24V und 0V)", "h1"))
story.append(P("Die +24V macht man nur <b>einmal</b> und verteilt sie dann am Relais:", "body"))
story.append(ListFlowable([
    ListItem(P("<b>Ein</b> Draht von X2 Klemme <b>2.8 (+24 V Geber)</b> &rarr; zu <b>CH1-COM</b>. "
               "In die SPS-Klemme 2.8 passt nur 1 Draht!", "body")),
    ListItem(P("Dann <b>4 kurze Bruecken</b> am Relais: CH1-COM &rarr; CH2-COM &rarr; CH3-COM &rarr; "
               "CH4-COM &rarr; CH5-COM. So bekommen alle 5 COM die gleichen 24V.", "body")),
], bulletType="bullet", start="square"))
story.append(Spacer(1, 3*mm))
story.append(P("ESP32-Pin (3,3V) &rarr; Relais IN (unten) &rarr; Relais-Kontakt schaltet &rarr; "
               "+24V von 2.8 &rarr; NO &rarr; X2 DI-Eingang", "mono"))
story.append(Spacer(1, 3*mm))
story.append(box("Das GND (0V) kommt NICHT ans Relais",
    "Der obere Relais-Teil ist nur ein <b>Schalter</b>. Der SPS-Eingang holt sich sein 0V "
    "<b>intern selbst</b> (zwischen Klemme 2.8 und 1.8 im X2). Du legst also <b>nur +24V auf die COM</b> "
    "und die <b>NO</b> auf die Eingaenge &ndash; <b>kein GND-Kabel ans Relais</b>.", "info"))
story.append(box("Wofuer ist GND (Klemme 1.8) dann?",
    "Nur fuer echte Sensoren mit eigener Stromversorgung &ndash; z.B. die Muenz-Lichtschranke "
    "(diMuenzeinwurf an DI 1). Die bekommt +24V (von 2.8), 0V (an 1.8) und ihr Signal (an DI 1, "
    "Klemme 1.1). Das Relais braucht das nicht.", "ok"))

# ---- 6. Warum ohne Widerstand ----
story.append(P("6. Direkt ohne Widerstand &ndash; was du wissen musst", "h1"))
story.append(P("Euer Lehrer hat es vereinfacht: <b>kein</b> Pulldown-Widerstand, NO geht direkt auf "
               "den SPS-Eingang. Das ist voellig in Ordnung &ndash; so musst du es machen.", "body"))
story.append(box("Was passiert ohne Widerstand?",
    "Schaltet das Relais (Fach leer), liegen die vollen <b>24V</b> am Eingang &ndash; sicheres "
    "&bdquo;EIN&ldquo;. Schaltet es nicht (Fach voll), ist der NO-Kontakt offen und der Eingang "
    "liegt auf 0 V der Eingangsgruppe &ndash; sicheres &bdquo;AUS&ldquo;. Der X20-Eingang ist robust "
    "genug, dass das ohne extra Widerstand sauber funktioniert.", "ok"))
story.append(box("Falls ein Eingang doch flackert",
    "Sollte ein Eingang in der Visu zufaellig springen, melde dich beim Lehrer &ndash; dann koennte "
    "im Einzelfall doch ein Pulldown noetig sein. Bei der direkten Verdrahtung wie gezeichnet ist das "
    "aber normalerweise kein Problem.", "warn"))

# ---- 7. Schritt fuer Schritt ----
story.append(P("7. Schritt fuer Schritt", "h1"))
steps = [
    "<b>Strom ausschalten.</b> Alle Netzteile aus, bevor du irgendetwas anschliesst.",
    "<b>X2 finden.</b> Mittlerer der 3 integrierten I/O-Steckplaetze an der CPU (siehe Kapitel 4).",
    "<b>24V messen (optional).</b> Multimeter zwischen 2.8 und 1.8 &ndash; sollte ca. 24V zeigen, wenn Strom an ist.",
    "<b>+24V auf die COMs.</b> Ein Draht X2-Klemme 2.8 &rarr; CH1-COM, dann Bruecken CH1&rarr;CH2&rarr;&hellip;&rarr;CH5 COM.",
    "<b>NO verkabeln.</b> CH1-NO &rarr; 2.1 (DI 2), CH2-NO &rarr; 1.2 (DI 3), CH3-NO &rarr; 2.2 (DI 4), "
    "CH4-NO &rarr; 1.3 (DI 5), CH5-NO &rarr; 2.3 (DI 6).",
    "<b>NC und CH6&ndash;CH8 frei lassen.</b> Nichts anschliessen. Keine Widerstaende, kein GND ans Relais.",
    "<b>Sichtkontrolle.</b> Sitzt alles fest? NO statt NC? COM-Bruecke durchgehend?",
    "<b>Einschalten &amp; testen.</b> Erst 24V (SPS), dann ESP32. In Automation Studio die Variablen "
    "diEspFach[0..3] und diEspNfc beobachten.",
]
story.append(ListFlowable(
    [ListItem(P(s, "step"), value=i+1) for i,s in enumerate(steps)],
    bulletType="1", leftIndent=18))

# ---- 8. So testest du ----
story.append(P("8. So pruefst du, ob es funktioniert", "h1"))
story.append(styled_table([
    header_row(["Aktion", "Erwartung in Automation Studio"]),
    [P("Fach 1 mit der Hand abdecken (Sensor sieht Produkt)", "cell"), P("diEspFach[0] wechselt, im Visu Fach 1 = voll", "cell")],
    [P("Fach 1 frei machen (kein Produkt)", "cell"), P("Fach 1 wird im Visu durchgestrichen / leer", "cell")],
    [P("Beliebige RFID-Karte an den NFC-Leser halten", "cell"), P("diEspNfc geht ~2 Sekunden auf 1 (erkannt)", "cell")],
], [75*mm, 75*mm]))
story.append(Spacer(1, 3*mm))
story.append(box("Signal verkehrt herum?",
    "Wenn ein volles Fach als leer angezeigt wird (oder umgekehrt): Du hast wahrscheinlich NC statt "
    "NO erwischt. Kabel von NC auf NO umstecken &ndash; <b>vorher Strom aus!</b>", "warn"))

# ---- 9. Troubleshooting ----
story.append(P("9. Problemloesung", "h1"))
story.append(styled_table([
    header_row(["Problem", "Wahrscheinliche Ursache", "Loesung"]),
    [P("SPS-Eingang reagiert gar nicht", "cell"), P("Kein 24V auf COM, oder NO/Eingang vertauscht", "cell"), P("24V an COM pruefen, NO-Verkabelung kontrollieren", "cell")],
    [P("Eingang flackert / springt", "cell"), P("Eingang haengt frei (selten)", "cell"), P("Lehrer fragen &ndash; evtl. doch ein Pulldown noetig", "cell")],
    [P("Alles invertiert (voll&harr;leer)", "cell"), P("NC statt NO benutzt", "cell"), P("Auf NO umklemmen", "cell")],
    [P("Relais klickt nicht", "cell"), P("ESP32-Seite (unten) nicht versorgt", "cell"), P("Freund fragen: VCC/JD-VCC/GND pruefen", "cell")],
    [P("Eingang dauerhaft auf EIN", "cell"), P("NO und COM verwechselt / Kurzschluss", "cell"), P("Verdrahtung CH-NO &rarr; Klemme pruefen", "cell")],
], [40*mm, 52*mm, 58*mm]))
story.append(Spacer(1, 6*mm))
story.append(hr())
story.append(P("Architektur-Hinweis: Der ESP32 steuert KEINE Motoren. Die Motoren schaltet die SPS "
               "selbst ueber Modul X3 (doMotor[0..3]). Dieses Relaismodul dient nur als "
               "Spannungs-Uebersetzer fuer die 5 Eingangssignale.", "small"))
story.append(P("Dokument: Verkabelung Relaismodul &rarr; B&amp;R X20 SPS &middot; Version 1 &middot; "
               "Projekt Swiity (HTL Voecklabruck)", "small"))

# ---------- Seitenzahlen ----------
def on_page(canv, doc):
    canv.saveState()
    canv.setStrokeColor(colors.HexColor("#d0d0d0")); canv.setLineWidth(0.5)
    canv.line(20*mm, 14*mm, 190*mm, 14*mm)
    canv.setFont("Helvetica", 8); canv.setFillColor(GREY)
    canv.drawString(20*mm, 9*mm, "Swiity  -  Verkabelung Relais zu SPS")
    canv.drawRightString(190*mm, 9*mm, f"Seite {doc.page}")
    canv.restoreState()

doc = BaseDocTemplate(OUT, pagesize=A4,
                      leftMargin=20*mm, rightMargin=20*mm,
                      topMargin=18*mm, bottomMargin=20*mm,
                      title="Verkabelung Relaismodul zu B&R X20 SPS",
                      author="Projekt Swiity")
frame = Frame(doc.leftMargin, doc.bottomMargin,
              doc.width, doc.height, id="main")
doc.addPageTemplates([PageTemplate(id="all", frames=[frame], onPage=on_page)])
doc.build(story)
print("PDF erstellt:", OUT)
