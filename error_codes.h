//VAG FEHLERCODES

#ifndef ERROR_CODES_H
#define ERROR_CODES_H
#include <avr/pgmspace.h>

typedef struct error_message{
	uint16_t code;
	char message[128];
}error_message_t;

typedef struct error{
	uint16_t code;
	uint8_t state;
}error_code_t;

static const error_message_t error_status[] PROGMEM = {
{ 1, "Signal an Plus."},
{ 2, "Signal an Masse."},
{ 3, "Kein Signal."},
{ 4, "Mechanischer Fehler."},
{ 5, "Eingang ge�ffnet."},
{ 6, "Signal zu stark."},
{ 7, "Signal zu schwach."},
{ 8, "Regelgrenze �berschritten."},
{ 9, "Anpassungsgrenze �berschritten."},
{ 10, "Anpassungsgrenze nicht erreicht."},
{ 11, "Regelgrenze nicht erreicht."},
{ 12, "Anpassungsgrenze (mul) �berschritten."},
{ 13, "Anpassungsgrenze (mul) nicht erreicht."},
{ 14, "Anpassungsgrenze (hinzuf�gen) �berschritten."},
{ 15, "Anpassungsgrenze (hinzuf�gen) nicht erreicht."},
{ 16, "Signal au�erhalb Toleranz."},
{ 17, "Regelungsabweichung."},
{ 18, "Oberes Anschlagventil."},
{ 19, "Unteres Anschlagventil."},
{ 20, "Fehler in Grundeinstellung."},
{ 21, "Druckaufbauzeit vorn zu lang."},
{ 22, "Druckabbauzeit vorn zu lang."},
{ 23, "Druckaufbauzeit hinten zu lang."},
{ 24, "Druckabbauzeit hinten zu lang."},
{ 25, "Undefinierter Schaltzustand."},
{ 26, "Ausgang ge�ffnet."},
{ 27, "Signal nicht plausibel."},
{ 28, "Kurzschluss gegen Plus."},
{ 29, "Masseschluss."},
{ 30, "Stromkreisunterbrechung/Kurzschluss gegen Plus."},
{ 31, "Stromkreisunterbrechung/Masseschluss."},
{ 32, "Widerstandswert zu gro�."},
{ 33, "Widerstandswert zu klein."},
{ 34, "Kein Fehlertyp identifiziert."},
{ 35, "??????????????"},
{ 36, "Stromkreisunterbrechung."},
{ 37, "Defekt."},
{ 38, "Ausgang schaltet nicht/Kurzschluss gegen Plus."},
{ 39, "Ausgang schaltet nicht/Masseschluss."},
{ 40, "Kurzschluss gegen anderes Ventil."},
{ 41, "Blockiert oder ohne Spannung."},
{ 42, "Drehzahlunterschied zu gro�."},
{ 43, "Geschlossen."},
{ 44, "Kurzschluss."},
{ 45, "Verbinder."},
{ 46, "Undicht."},
{ 47, "Falscher Anschluss."},
{ 48, "Spannungsversorgung."},
{ 49, "Keine Kommunikation."},
{ 50, "Position fr�h nicht erreicht."},
{ 51, "Position sp�t nicht erreicht."},
{ 52, "Versorgungsspannung zu hoch."},
{ 53, "Versorgungsspannung zu gering."},
{ 54, "Falsche Ger�te."},
{ 55, "Anpassung nicht in Notmodus."},
{ 57, "Stromkreisfehler."},
{ 58, "Keine Verriegelung."},
{ 59, "Keine Entriegelung."},
{ 60, "Doppelverriegelung arretiert nicht."},
{ 61, "Doppelverriegelung l�st nicht."},
{ 62, "Keine oder falsche Einstellung."},
{ 63, "Temperaturabschaltung."},
{ 67, "Sollwert nicht erreicht."},
{ 68, "Zy1."},
{ 69, "Zy1."},
{ 70, "Zy1."},
{ 71, "Zy1."},
{ 72, "Zy1."},
{ 73, "ZY1."},
{ 74, "ZY1."},
{ 75, "ZY1."},
{ 76, "Klemme 30 fehlt."},
{ 77, "Interne Versorgungsspannung."},
{ 78, "Meldung fehlt."},
{ 79, "Fehlerspeicher abfragen."},
{ 80, "In Eindraht-Betriebsmodus."},
{ 129, "Signal an Plus/Sporadischer Fehler."},
{ 130, "Signal an Masse/Sporadischer Fehler."},
{ 131, "Kein Signal/Sporadischer Fehler."},
{ 132, "Mechanischer Fehler/Sporadischer Fehler."},
{ 133, "Eingang ge�ffnet/Sporadischer Fehler."},
{ 134, "Signal zu stark/Sporadischer Fehler."},
{ 135, "Signal zu schwach/Sporadischer Fehler."},
{ 136, "Regelgrenze �berschritten/Sporadischer Fehler."},
{ 137, "Anpassungsgrenze �berschritten/Sporadischer Fehler."},
{ 138, "Anpassungsgrenze nicht erreicht/Sporadischer Fehler."},
{ 139, "Regelgrenze nicht erreicht/Sporadischer Fehler."},
{ 140, "Anpassungsgrenze (mul) �berschritten/Sporadischer Fehler."},
{ 141, "Anpassungsgrenze (mul) nicht erreicht/Sporadischer Fehler."},
{ 142, "Anpassungsgrenze (hinzuf�gen) �berschritten/Sporadischer Fehler."},
{ 143, "Anpassungsgrenze (hinzuf�gen) nicht erreicht/Sporadischer Fehler."},
{ 144, "Signal au�erhalb Toleranz/Sporadischer Fehler."},
{ 145, "Regelungsabweichung/Sporadischer Fehler."},
{ 146, "Oberes Anschlagventil/Sporadischer Fehler."},
{ 147, "Unteres Anschlagventil/Sporadischer Fehler."},
{ 148, "Fehler in Grundeinstellung/Sporadischer Fehler."},
{ 149, "Druckaufbauzeit vorn zu lang/Sporadischer Fehler."},
{ 150, "Druckabbauzeit vorn zu lang/Sporadischer Fehler."},
{ 151, "Druckaufbauzeit hinten zu lang/Sporadischer Fehler."},
{ 152, "Druckabbauzeit hinten zu lang/Sporadischer Fehler."},
{ 153, "Undefinierter Schaltzustand/Sporadischer Fehler."},
{ 154, "Ausgang ge�ffnet/Sporadischer Fehler."},
{ 155, "Signal nicht plausibel/Sporadischer Fehler."},
{ 156, "Kurzschluss gegen Plus/Sporadischer Fehler."},
{ 157, "Masseschluss/Sporadischer Fehler."},
{ 158, "Stromkreisunterbrechung/Kurzschluss gegen Plus/Sporadischer Fehler."},
{ 159, "Stromkreisunterbrechung/Masseschluss/Sporadischer Fehler."},
{ 160, "Widerstandswert zu gro�/Sporadischer Fehler."},
{ 161, "Widerstandswert zu klein/Sporadischer Fehler."},
{ 162, "Kein Fehlertyp identifiziert/Sporadischer Fehler."},
{ 163, "Sporadischer Fehler."},
{ 164, "Stromkreisunterbrechung/Sporadischer Fehler."},
{ 165, "Defekt/Sporadischer Fehler."},
{ 166, "Ausgang schaltet nicht/Kurzschluss gegen Plus/Sporadischer Fehler."},
{ 167, "Ausgang schaltet nicht/Masseschluss/ Sporadischer Fehler."},
{ 168, "Kurzschluss gegen anderes Ventil/Sporadischer Fehler."},
{ 169, "Blockiert oder ohne Spannung/Sporadischer Fehler."},
{ 170, "Drehzahlunterschied zu gro�/Sporadischer Fehler."},
{ 171, "Geschlossen/Sporadischer Fehler."},
{ 172, "Kurzschluss/Sporadischer Fehler."},
{ 173, "Verbinder/Sporadischer Fehler."},
{ 174, "Undicht/Sporadischer Fehler."},
{ 175, "Falscher Anschluss/Sporadischer Fehler."},
{ 176, "Spannungsversorgung/Sporadischer Fehler."},
{ 177, "Keine Kommunikation/Sporadischer Fehler."},
{ 178, "Position fr�h nicht erreicht/Sporadischer Fehler."},
{ 179, "Position sp�t nicht erreicht/Sporadischer Fehler."},
{ 180, "Versorgungsspannungs zu hoch/Sporadischer Fehler."},
{ 181, "Versorgungsspannungs zu gering/Sporadischer Fehler."},
{ 182, "Falsche Ger�te/Sporadischer Fehler."},
{ 183, "Anpassung nicht in Notmodus/Sporadischer Fehler."},
{ 185, "Stromkreisfehler/Sporadischer Fehler."},
{ 186, "Keine Verriegelung/Sporadischer Fehler."},
{ 187, "Keine Entriegelung/Sporadischer Fehler."},
{ 188, "Doppelverriegelung arretiert nicht/Sporadischer Fehler."},
{ 189, "Doppelverriegelung l�st nicht/Sporadischer Fehler."},
{ 190, "Keine oder falsche Einstellung/Sporadischer Fehler."},
{ 191, "Temperaturabschaltung/Sporadischer Fehler."},
{ 192, "Kann derzeit nicht gepr�ft werden."},
{ 193, "Nicht befugt."},
{ 194, "Ausgleich nicht durchgef�hrt."},
{ 195, "Sollwert nicht erreicht/Sporadischer Fehler."},
{ 196, "Zy1/Sporadischer Fehler."},
{ 197, "Zy1/Sporadischer Fehler."},
{ 198, "Zy1/Sporadischer Fehler."},
{ 199, "Zy1/Sporadischer Fehler."},
{ 200, "Zy1/Sporadischer Fehler."},
{ 201, "ZY1/Sporadischer Fehler."},
{ 202, "ZY1/Sporadischer Fehler."},
{ 203, "ZY1/Sporadischer Fehler."},
{ 204, "Klemme 30 fehlt/Sporadischer Fehler."},
{ 205, "Interne Spannungsversorgung/Sporadischer Fehler."},
{ 206, "Meldung fehlt/Sporadischer Fehler."},
{ 207, "Fehlerspeicher abfragen/Sporadischer Fehler."},
{ 208, "In Eindraht-Betriebsmodus/Sporadischer Fehler."}
};

static const error_message_t engine_errors_low[] PROGMEM = {
{281, "Geber f�r Fahrgeschwindigkeit-G68"},
{282, "Drosselklappensteller-V60"},
{513, "Geber f�r Motordrehzahl-G28"},
{515, "Hallgeber-G40"},
{516, "Leerlaufschalter-F60"},
{518, "Drosselklappenpotentiometer-G69"},
{519, "Geber f�r Saugrohrdruck-G71"},
{522, "Geber f�r K�hlmitteltemperatur-G62"},
{524, "Klopfsensor 1-G61"},
{525, "Lambda-Sonde-G39"},
{527, "Geber f�r Saugrohrtemperatur-G72"},
{530, "Drosselklappensteller/Potentiometer-G88"},
{532, "Versorgungsspannung"},
{533, "Leerlaufregelung"},
{537, "Lambda-Regelung"},
{539, "Geber f�r Kraftstofftemperatur-G81"},
{540, "Klopfsensor 2-G66"},
{542, "Geber f�r Nadelhub-G80"},
{543, "Drehzahl nach Maximum �berschritten"},
{545, "Motor/Getriebe elektrische Verbindung"},
{550, "Spritzbeginnregelung"},
{553, "Luftmassenmesser-G70"},
{560, "Abgasr�ckf�hrungssystem"},
{560, "Abgasr�ckf�hrungssystem"},
{561, "Gemischanpassung"},
{575, "Saugrohrdruck"},
{577, "Klopfregelung Zylinder 1"},
{578, "Klopfregelung Zylinder 2"},
{579, "Klopfregelung Zylinder 3"},
{580, "Klopfregelung Zylinder 4"},
{581, "Klopfregelung Zylinder 5"},
{582, "Klopfregelung Zylinder 6"},
{585, "Temperaturf�hler Abgasr�ckf�hrung-G98"},
{625, "Geschwindigkeitssignal"},
{626, "Kontrolllampe f�r Vorgl�hzeit-K29"},
{635, "Heizung Lambdasonde vor Katalysator"},
{668, "Bordspannung Klemme 30"},
{671, "Schalter f�r GRA-E45"},
{703, "Leerlauferkennung Drosselklappenpotentiometer/Leerlaufschalter"},
{740, "Geber f�r Zylinder 1 Erkennung-G145"},
{741, "Bremspedal�berwachung"},
{758, "Sekund�rluftsystem"},
{765, "Geber f�r Regelschieberweg-G149"},
{777, "Geber f�r Gaspedalstellung-G79"},
{1044, "Steuerger�t falsch codiert"},
{1087, "Grundeinstellung nicht durchgef�hrt"},
{1117, "Belastungssignal f�r Generatorklemme DF"},
{1119, "Signal f�r Gangerkennung"},
{1165, "Drosselklappensteuereinheit-J338"},
{1180, "Motor/Klimakompressorabschaltung elektrische Verbindung"},
{1208, "Datens�tze im Steuerger�t ver�ndert"},
{1235, "Sekund�rlufteinblasventil-N112"},
{1237, "Kraftstoffabschaltventil-N109"},
{1242, "Endstufen im Steuerger�t"},
{1247, "Magnetventil 1 f�r Aktivkohlebeh�lter-N80"},
{1249, "Einspritzventil Zylinder 1-N30"},
{1250, "Einspritzventil Zylinder 2-N31"},
{1251, "Einspritzventil Zylinder 3-N32"},
{1252, "Einspritzventil Zylinder 4-N33"},
{1253, "Einspritzventil Zylinder 5-N83"},
{1254, "Einspritzventil Zylinder 6-N84"},
{1259, "Kraftstoffpumpenrelais-J17"},
{1262, "Magnetventil Ladedruckbegrenzung-N75"},
{1265, "Ventil f�r Abgasr�ckf�hrung-N18"},
{1266, "Relais f�r Gl�hkerzen-J52"},
{1268, "Mengensteller-N146"},
{1269, "Ventil f�r Einspritzbeginn-N108"}
};
static const error_message_t engine_errors_high[] PROGMEM = {
{16398, "P0014 Bank1,Nockenwellensp�tverstellung,Auslass Soll nicht erreicht"},
{16399, "P0015 Bank1,Nockenwellenfr�hverstellung,Auslass Soll nicht erreicht"},
{16485, "P0101 Luftmassenmesser-G70 unplausibles Signal"},
{16486, "P0102 Luftmassenmesser-G70 Signal zu klein"},
{16487, "P0103 Luftmassenmesser-G70 Signal zu gro�"},
{16496, "P0112 Geber f�r Ansauglufttemperatur-G42 Signal zu klein"},
{16497, "P0113 Geber f�r Ansauglufttemperatur-G42 Signal zu gro�"},
{16500, "P0116 Geber f�r K�hlmitteltemperatur-G62 unplausibles Signal"},
{16501, "P0117 Geber f�r K�hlmitteltemperatur-G62 Signal zu klein"},
{16502, "P0118 Geber f�r K�hlmitteltemperatur-G62 Signal zu gro�"},
{16514, "P0130 Bank1-Sonde1 elektrischer Fehler im Stromkreis"},
{16515, "P0131 Bank1-Sonde1 Spannung zu klein"},
{16516, "P0132 Bank1-Sonde1 Spannung zu gro�"},
{16517, "P0133 Bank1-Sonde1 Signal zu langsam"},
{16518, "P0134 Bank1-Sonde1 keine Aktivit�t"},
{16520, "P0136 Bank1-Sonde2 elektrischer Fehler im Stromkreis"},
{16521, "P0137 Bank1-Sonde2 Spannung zu klein"},
{16522, "P0138 Bank1-Sonde2 Spannung zu gro�"},
{16523, "P0139 Bank1-Sonde2 Signal zu langsam"},
{16524, "P0140 Bank1-Sonde2 keine Aktivit�t"},
{16554, "P0170 Bank1,Kraftstoffbemessungssystem Fehlfunktion"},
{16555, "P0171 Bank1,Kraftstoffbemessungssystem System zu mager"},
{16556, "P0172 Bank1,Kraftstoffbemessungssystem System zu fett"},
{16585, "P0201 Einspritzventil Zylinder 1-N30 elektrischer Fehler im Stromkreis"},
{16586, "P0202 Einspritzventil Zylinder 2-N31 elektrischer Fehler im Stromkreis"},
{16587, "P0203 Einspritzventil Zylinder 3-N32 elektrischer Fehler im Stromkreis"},
{16588, "P0204 Einspritzventil Zylinder 4-N33 elektrischer Fehler im Stromkreis"},
{16589, "P0205 Einspritzventil Zylinder 5-N33 elektrischer Fehler im Stromkreis"},
{16590, "P0206 Einspritzventil Zylinder 6-N33 elektrischer Fehler im Stromkreis"},
{16603, "P0219 maximale Motordrehzahl �berschritten"},
{16645, "P0261 Einspritzventil Zylinder 1-N30 Kurzschluss nach Masse"},
{16646, "P0262 Einspritzventil Zylinder 1-N30 Kurzschluss nach Plus"},
{16648, "P0264 Einspritzventil Zylinder 2-N31 Kurzschluss nach Masse"},
{16651, "P0267 Einspritzventil Zylinder 3-N32 Kurzschluss nach Masse"},
{16652, "P0268 Einspritzventil Zylinder 3-N32 Kurzschluss nach Plus"},
{16654, "P0270 Einspritzventil Zylinder 4-N33 Kurzschluss nach Masse"},
{16655, "P0271 Einspritzventil Zylinder 4-N33 Kurzschluss nach Plus"},
{16657, "P0273 Einspritzventil Zylinder 5-N33 Kurzschluss nach Masse"},
{16658, "P0274 Einspritzventil Zylinder 5-N33 Kurzschluss nach Plus"},
{16660, "P0276 Einspritzventil Zylinder 6-N33 Kurzschluss nach Masse"},
{16661, "P0277 Einspritzventil Zylinder 6-N33 Kurzschluss nach Plus"},
{16684, "P0300 Verbrennungsaussetzer erkannt"},
{16685, "P0301 Zylinder 1 Verbrennungsaussetzer erkannt"},
{16686, "P0302 Zylinder 2 Verbrennungsaussetzer erkannt"},
{16687, "P0303 Zylinder 3 Verbrennungsaussetzer erkannt"},
{16688, "P0304 Zylinder 4 Verbrennungsaussetzer erkannt"},
{16689, "P0305 Zylinder 5 Verbrennungsaussetzer erkannt"},
{16690, "P0306 Zylinder 6 Verbrennungsaussetzer erkannt"},
{16705, "P0321 Geber f�r Motordrehzahl-G28 unplausibles Signal"},
{16706, "P0322 Geber f�r Motordrehzahl-G28 kein Signal"},
{16711, "P0327 Klopfsensor 1-G61 Signal zu klein"},
{16712, "P0328 Klopfsensor 1-G61 Signal zu gro�"},
{16716, "P0332 Klopfsensor 2-G66 Signal zu klein"},
{16717, "P0333 Klopfsensor 2-G66 Signal zu gro�"},
{16719, "P0335 Geber f�r Motordrehzahl-G28 Fehlfunktion"},
{16725, "P0341 Nockenwellenpositionssensor=>Geber-G40 unplausibles Signal"},
{16726, "P0342 Nockenwellenpositionssensor=>Geber-G40 Signal zu klein"},
{16727, "P0343 Nockenwellenpositionssensor=>Geber-G40 Signal zu gro�"},
{16730, "P0346 Nockenwellenpositionssensor=>Geber-G163 unplausibles Signal"},
{16795, "P0411 Sekund�rluftsystem Durchfluss fehlerhaft"},
{16804, "P0420 Bank1,Katalysatorsystem Wirkung zu gering"},
{16825, "P0441 Tankentl�ftungssystem Durchsatz fehlerhaft"},
{16845, "P0461 Kraftstofff�llstandssignal unplausibles Signal"},
{16885, "P0501 Fahrzeug-Geschwindigkeitssignal unplausibles Signal"},
{16890, "P0506 Leerlaufregelung Drehzahl unter Sollwert"},
{16891, "P0507 Leerlaufregelung Drehzahl �ber Sollwert"},
{16916, "P0532 Drucksensor f�r Klimaanlage-G65 Signal zu klein"},
{16917, "P0533 Drucksensor f�r Klimaanlage-G65 Signal zu gro�"},
{16944, "P0560 Spannungsversorgung unplausibles Signal"},
{16946, "P0562 Spannungsversorgung Spannung zu klein"},
{16947, "P0563 Spannungsversorgung Spannung zu gro�"},
{16955, "P0571 Bremslichtschalter-F unplausibles Signal"},
{16984, "P0600 Datenbus Antrieb fehlende Botschaft"},
{16985, "P0601 Steuerger�t defekt"},
{16988, "P0604 Steuerger�t defekt"},
{16989, "P0605 Steuerger�t defekt"},
{16990, "P0606 Steuerger�t defekt"},
{17029, "P0645 Klimaanlage Eingang/Ausgang"},
{17510, "P1102 Bank1-Sonde1,Heizstromkreis Kurzschluss nach Plus"},
{17511, "P1103 Bank1-Sonde1,Heizstromkreis Leistung zu gering"},
{17513, "P1105 Bank1-Sonde2,Heizstromkreis Kurzschluss nach Plus"},
{17523, "P1115 Bank1-Sonde1,Heizstromkreis Kurzschluss nach Masse"},
{17524, "P1116 Bank1-Sonde1,Heizstromkreis Unterbrechung"},
{17525, "P1117 Bank1-Sonde2,Heizstromkreis Kurzschluss nach Masse"},
{17526, "P1118 Bank1-Sonde2,Heizstromkreis Unterbrechung"},
{17549, "P1141 Lasterfassung unplausibler Wert"},
{17552, "P1144 Luftmassenmesser-G70 Unterbrechung/Kurzschluss nach Masse"},
{17553, "P1145 Luftmassenmesser-G70 Kurzschluss nach Plus"},
{17554, "P1146 Luftmassenmesser-G70 Versorgungsspannung"},
{17563, "P1155 Geber f�r Saugrohrdruck-G71 Kurzschluss nach Plus"},
{17564, "P1156 Geber f�r Saugrohrdruck-G71 Unterbrechung/Kurzschluss nach Masse"},
{17565, "P1157 Geber f�r Saugrohrdruck-G71 Versorgungsspannung"},
{17568, "P1160 Geber f�r Saugrohrtemperatur-G72 Kurzschluss nach Masse"},
{17569, "P1161 Geber f�r Saugrohrtemperatur-G72 Unterbrechung/Kurzschluss nach Plus"},
{17570, "P1162 Geber f�r Kraftstofftemperatur-G81 Kurzschluss nach Masse"},
{17571, "P1163 Geber f�r Kraftstofftemperatur-G81 Unterbrechung/Kurzschluss nach Plus"},
{17579, "P1171 Winkelgeber 2 f�r Drosselklappenantrieb-G188 unplausibles Signal"},
{17580, "P1172 Winkelgeber 2 f�r Drosselklappenantrieb-G188 Signal zu klein"},
{17581, "P1173 Winkelgeber 2 f�r Drosselklappenantrieb-G188 Signal zu gro�"},
{17584, "P1176 Bank1,Lambdakorrektur hinter Katalysator Regelgrenze erreicht"},
{17621, "P1213 Einspritzventil Zylinder 1-N30 Kurzschluss nach Plus"},
{17622, "P1214 Einspritzventil Zylinder 2-N31 Kurzschluss nach Plus"},
{17623, "P1215 Einspritzventil Zylinder 3-N32 Kurzschluss nach Plus"},
{17624, "P1216 Einspritzventil Zylinder 4-N33 Kurzschluss nach Plus"},
{17625, "P1217 Einspritzventil Zylinder 5-N83 Kurzschluss nach Plus"},
{17633, "P1225 Einspritzventil Zylinder 1-N30 Kurzschluss nach Masse"},
{17634, "P1226 Einspritzventil Zylinder 2-N31 Kurzschluss nach Masse"},
{17635, "P1227 Einspritzventil Zylinder 3-N32 Kurzschluss nach Masse"},
{17636, "P1228 Einspritzventil Zylinder 4-N33 Kurzschluss nach Masse"},
{17637, "P1229 Einspritzventil Zylinder 5-N83 Kurzschluss nach Masse"},
{17645, "P1237 Einspritzventil Zylinder 1-N30 Unterbrechung"},
{17646, "P1238 Einspritzventil Zylinder 2-N31 Unterbrechung"},
{17647, "P1239 Einspritzventil Zylinder 3-N32 Unterbrechung"},
{17648, "P1240 Einspritzventil Zylinder 4-N33 Unterbrechung"},
{17649, "P1241 Einspritzventil Zylinder 5-N83 Unterbrechung"},
{17653, "P1245 Geber f�r Nadelhub-G80 Kurzschluss nach Masse"},
{17654, "P1246 Geber f�r Nadelhub-G80 unplausibles Signal"},
{17655, "P1247 Geber f�r Nadelhub-G80 Unterbrechung/Kurzschluss nach Plus"},
{17656, "P1248 Spritzbeginnregelung Regeldifferenz"},
{17659, "P1251 Ventil f�r Einspritzbeginn-N108 Kurzschluss nach Plus"},
{17660, "P1252 Ventil f�r Einspritzbeginn-N108 Unterbrechung/Kurzschluss nach Masse"},
{17663, "P1255 Geber f�r K�hlmitteltemperatur-G62 Kurzschluss nach Masse"},
{17664, "P1256 Geber f�r K�hlmitteltemperatur-G62 Unterbrechung/Kurzschluss nach Plus"},
{17743, "P1335 Motormomenten�berwachung 2 Regelgrenze �berschritten"},
{17744, "P1336 Motormomenten�berwachung Regelgrenze �berschritten"},
{17749, "P1341 Z�ndausgang 1 Kurzschluss nach Masse"},
{17762, "P1354 Geber f�r Regelschieberweg-G149 elektrischer Fehler im Stromkreis"},
{17794, "P1386 Steuerger�t defekt"},
{17795, "P1387 Steuerger�t defekt"},
{17796, "P1388 Steuerger�t defekt"},
{17801, "P1393 Z�ndausgang 1 elektrischer Fehler im Stromkreis"},
{17805, "P1397 Geberrad f�r Motordrehzahl Adaptionsgrenze erreicht"},
{17810, "P1402 Ventil f�r Abgasr�ckf�hrung-N18 Kurzschluss nach Plus"},
{17818, "P1410 Tankentl�ftungsventil-N80 Kurzschluss nach Plus"},
{17833, "P1425 Tankentl�ftungsventil-N80 Kurzschluss nach Masse"},
{17834, "P1426 Tankentl�ftungsventil-N80 Unterbrechung"},
{17849, "P1441 Ventil f�r Abgasr�ckf�hrung-N18 Unterbrechung/Kurzschluss nach Masse"},
{17909, "P1501 Kraftstoffpumpenrelais-J17 Kurzschluss nach Masse"},
{17910, "P1502 Kraftstoffpumpenrelais-J17 Kurzschluss nach Plus"},
{17911, "P1503 Belastungssignal von Generatorklemme DF unplausibles Signal"},
{17915, "P1507 Leerlaufsystem-Lernwert untere Grenze erreicht"},
{17916, "P1508 Leerlaufsystem-Lernwert obere Grenze erreicht"},
{17931, "P1523 Crash-Signal vom Airbag-Steuerger�t unplausibles Signal"},
{17945, "P1537 Kraftstoffabschaltventil-N109 Fehlfunktion"},
{17946, "P1538 Kraftstoffabschaltventil-N109 Unterbrechung/Kurzschluss nach Masse"},
{17947, "P1539 Kupplungspedalschalter-F36 unplausibles Signal"},
{17948, "P1540 Fahrzeug-Geschwindigkeitssignal Signal zu gro�"},
{17949, "P1541 Kraftstoffpumpenrelais-J17 Unterbrechung"},
{17950, "P1542 Winkelgeber f�r DK-Antrieb-G187 unplausibles Signal"},
{17951, "P1543 Winkelgeber f�r DK-Antrieb-G187 Signal zu klein"},
{17952, "P1544 Winkelgeber f�r DK-Antrieb-G187 Signal zu gro�"},
{17953, "P1545 Drosselklappensteuerung Fehlfunktion"},
{17954, "P1546 Magnetventil f�r Ladedruckbegrenzung-N75 Kurzschluss nach Plus"},
{17957, "P1549 Magnetventil f�r Ladedruckbegrenzung-N75 Unterbrechung/Kurzschluss nach Masse"},
{17964, "P1556 Ladedruckregelung Regelgrenze unterschritten"},
{17965, "P1557 Ladedruckregelung Regelgrenze �berschritten"},
{17966, "P1558 Drosselklappenantrieb-G186 elektrischer Fehler im Stromkreis"},
{17967, "P1559 Drosselklappensteuereinheit-J338 Fehler in Grundeinstellung"},
{17969, "P1561 Mengensteller-N146 Regeldifferenz"},
{17970, "P1562 Mengensteller-N146 oberer Anschlagwert"},
{17971, "P1563 Mengensteller-N146 unterer Anschlagwert"},
{17972, "P1564 Drosselklappensteuereinheit-J338 Unterspannung bei Grundeinstellung"},
{17973, "P1565 Drosselklappensteuereinheit-J338 unterer Anschlag wird nicht erreicht"},
{17976, "P1568 Drosselklappensteuereinheit-J338 mechanischer Fehler"},
{17977, "P1569 Schalter f�r GRA-E45 unplausibles Signal"},
{17978, "P1570 Motorsteuerger�t gesperrt"},
{17987, "P1579 Drosselklappensteuereinheit-J338 Adaption nicht gestartet"},
{18008, "P1600 Spannungsversorgung Klemme 15 Spannung zu klein"},
{18009, "P1601 Relais f�r Spannungsversorgung-Klemme 30-J317 unplausibles Signal"},
{18010, "P1602 Spannungsversorgung Klemme 30 Spannung zu klein"},
{18011, "P1603 Steuerger�t defekt"},
{18014, "P1606 Schlechtwegeinfo/Motorsollmoment vom ABS-Steuerger�t elektrischer Fehler im Stromkreis"},
{18020, "P1612 Motorsteuerger�t falsch codiert"},
{18024, "P1616 Kontrolllampe f�r Vorgl�hzeit-K29 Kurzschluss nach Plus"},
{18025, "P1617 Kontrolllampe f�r Vorgl�hzeit-K29 Unterbrechung/Kurzschluss nach Masse"},
{18026, "P1618 Relais f�r Gl�hkerzen-J52 Kurzschluss nach Plus"},
{18027, "P1619 Relais f�r Gl�hkerzen-J52 Unterbrechung/Kurzschluss nach Masse"},
{18033, "P1625 Datenbus Antrieb unplausible Botschaft vom Getriebe-Steuerger�t"},
{18034, "P1626 Datenbus Antrieb fehlende Botschaft vom Getriebe-Steuerger�t"},
{18038, "P1630 Geber f�r Gaspedalstellung-G79 Signal zu klein"},
{18039, "P1631 Geber f�r Gaspedalstellung-G79 Signal zu gro�"},
{18040, "P1632 Geber f�r Gaspedalstellung-G79 Versorgungsspannung"},
{18041, "P1633 Geber 2 f�r Gaspedalstellung-G185 Signal zu klein"},
{18042, "P1634 Geber 2 f�r Gaspedalstellung-G185 Signal zu gro�"},
{18047, "P1639 Geber 1/2 f�r Gaspedalstellung-G79+G185 unplausibles Signal"},
{18048, "P1640 Steuerger�t defekt"},
{18056, "P1648 Datenbus Antrieb defekt"},
{18057, "P1649 Datenbus Antrieb fehlende Botschaft vom ABS-Steuerger�t"},
{18058, "P1650 Datenbus Antrieb fehlende Botschaft vom Kombiinstrument"},
{18061, "P1653 Bitte Fehlerspeicher des ABS-Steuerger�t auslesen"},
{18062, "P1654 Bitte Fehlerspeicher des Kombiinstrumentes auslesen"},
{18064, "P1656 Klimaanlage Eingang/Ausgang Kurzschluss nach Masse"},
{18065, "P1657 Klimaanlage Eingang/Ausgang Kurzschluss nach Plus"},
{18085, "P1677 Fehlerlampe f�r elektrische Gasbet�tigung-K132 Kurzschluss nach Plus"},
{18086, "P1678 Fehlerlampe f�r elektrische Gasbet�tigung-K132 Kurzschluss nach Masse"},
{18087, "P1679 Fehlerlampe f�r elektrische Gasbet�tigung-K132 Unterbrechung"},
{18088, "P1680 Resetfester Notlauf aktiv"},
{18090, "P1682 Datenbus-Antrieb unplausible Botschaft vom ABS-SG"},
{18100, "P1692 Fehlerlampe f�r Eigendiagnose-K83 Kurzschluss nach Masse"},
{18261, "P1853 Datenbus Antrieb unplausible Botschaft vom ABS-Steuerger�t"},
{19458, "P3002 Kick-Down-Schalter-F8 unplausibles Signal"},
{19560, "P3104 Umschaltventil f�r Saugrohrklappe-N239 Kurzschluss nach Plus"},
{19561, "P3105 Umschaltventil f�r Saugrohrklappe-N239 Unterbrechung/Kurzschluss nach Masse"},
{65535, "Steuerger�t defekt"}

};
/* Fehler Climatronic*/
static const error_message_t climatronic_errors[] PROGMEM = {
{532, "Versorgungsspannung"},
{538, "Referenzspannung"},
{565, "Bedienungs- und Anzeigeeinheit f�r Climatronic hinten-E265"},
{603, "Stellmotor f�r Fu�raum-/Defrostklappe-V85"},
{631, "Verbindung AC - Motronic"},
{658, "Bedienungseinheit"},
{705, "Relais f�r K�hlerl�fter Stufe 1-J279"},
{706, "Relais f�r K�hlerl�fter Stufe 2-J513"},
{707, "Ventil f�r Mitteld�sen-N236"},
{739, "Pumpe f�r K�hlmittelumlauf-V50"},
{766, "Geber f�r Ausstr�mtemperatur vorn-G152"},
{767, "Geber f�r Verdampfertemperatur hinten-G153"},
{768, "Geber f�r W�rmetauschertemperatur hinten-G154"},
{779, "Temperaturf�hler Aussentemperatur-G17"},
{791, "Temperaturschalter f�r Verdampfer-E33"},
{792, "Druckschalter f�r Klimaanlage-F129"},
{796, "Gebl�se f�r Temperaturf�hler-V42"},
{797, "Fotosensor f�r Sonneneinstrahlung-G107"},
{799, "Geber f�r K�hlmitteltemperatur-G110"},
{1026, "Gebl�se f�r Verdampfer hinten-V20"},
{1027, "Warmluftgebl�se hinten-V47"},
{1028, "Relais f�r Gebl�se-J323"},
{1086, "Geber f�r Geschwindigkeitsmesser-G22"},
{1086, "Geber f�r Geschwindigkeitsmesser-G22"},
{1205, "Relais f�r Climatronic-J254"},
{1271, "Stellmotor f�r Temperaturklappe-V68"},
{1272, "Stellmotor f�r Zentralklappe-V70"},
{1273, "Frischluftgebl�se-V2"},
{65535, "Steuerger�t defekt"}
};
/* Fehler WFS*/
static const error_message_t wfs_errors[] PROGMEM = {
{546, "Datenleitung defekt"},
{1128, "Lesespule f�r Wegfahrsicherung-D2"},
{1176, "Schl�ssel"},
{1177, "Motorsteuerger�t"},
{1179, "Schl�sselprogrammierung falsch"},
{1181, "Initialisierung: nur 2 Schl�ssel gelernt"},
{1202, "Diagnoseleitung"},
{1237, "Kraftstoffabschaltventil-N109"},
{1312, "Daten-BUS Antrieb"},
{1314, "Motorsteuerger�t"},
{65535, "Steuerger�t defekt"}
};

/* Fehler Schalttafeleinsatz*/
static const error_message_t ki_errors[] PROGMEM = {
{562, "Geber f�r Oelstand-/temperatur-G266"},
{534, "Geber f�r Oeltemperatur-G8"},
{667, "Au�entemperatursignal"},
{771, "Geber f�r Kraftstoffvorratsanzeige-G"},
{778, "Geber f�r Lenkwinkel-G85"},
{779, "Temperaturf�hler Aussentemperatur-G17"},
{1039, "Geber f�r K�hlmitteltemperaturanzeige-G2"},
{1044, "Steuerger�t falsch codiert"},
{1312, "Daten-BUS Antrieb"},
{1314, "Motorsteuerger�t"},
{1315, "Getriebesteuerger�t"},
{1316, "Bremsensteuerger�t"},
{1317, "Steuerger�t im Schalttafeleinsatz-J285"},
{1321, "Steuerger�t f�r Airbag-J234"},
{1402, "Datenleitung von Navigation"},
{65535, "Steuerger�t defekt"}
};

static const error_message_t abs_errors[] PROGMEM = {
/* Fehler ABS*/
{257, "Einlassventil ABS vorn links-N101"},
{259, "Einlassventil ABS vorn rechts-N99"},
{265, "Auslassventil ABS vorn links-N102"},
{267, "Auslassventil ABS vorn rechts-N100"},
{273, "Einlassventil ABS hinten rechts-N133"},
{274, "Einlassventil ABS hinten links-N134"},
{275, "Auslassventil ABS hinten rechts-N135"},
{276, "Auslassventil ABS hinten links-N136"},
{283, "Drehzahlf�hler vorn links-G47"},
{285, "Drehzahlf�hler vorn rechts-G45"},
{287, "Drehzahlf�hler hinten rechts-G44"},
{290, "Drehzahlf�hler hinten links-G46"},
{301, "R�ckf�rderpumpe f�r ABS-V39"},
{526, "Bremslichtschalter-F"},
{532, "Versorgungsspannung"},
{642, "EDS Umschaltventil vorn rechts-N166"},
{643, "EDS Auslassventil vorn rechts-N167"},
{644, "EDS Umschaltventil vorn links-N168"},
{645, "EDS Auslassventil vorn links-N169"},
{1130, "ABS-Betrieb"},
{1200, "Versorgungsspannung f�r ABS-Ventile"},
{1201, "Versorgungsspannung f�r ABS-Pumpe"},
{65535, "Steuerger�t defekt"}
};


/* Fehler Airbag*/
static const error_message_t airbag_errors[] PROGMEM = {
{532, "Versorgungsspannung"},
{588, "Z�nder f�r Airbag (Fahrerseite)-N95"},
{589, "Z�nder 1 f�r Airbag (Beifahrerseite)-N131"},
{594, "Z�ndkreise Airbag"},
{595, "Crashdaten gespeichert"},
{1025, "Ansteuerung Fehlerlampe defekt"},
{1280, "Airbag Beifahrerseite abgeschaltet"},
{65535, "Steuerger�t defekt"}
};

static const error_message_t zv_errors[] PROGMEM = {
/* Fehler ZV*/
{949, "Motor f�r Zentralverriegelung Heckklappe-V53,LOCK"},
{950, "Motor f�r Zentralverriegelung Heckklappe-V53,UNLOCK"},
{955, "Schl�ssel 1"},
{956, "Schl�ssel 2"},
{957, "Schl�ssel 3"},
{958, "Schl�ssel 4"},
{1038, "Zentralverriegelung Thermoschutz"},
{1044, "Steuerger�t falsch codiert"},
{1131, "Ansteuerung Blinkleuchten"},
{1179, "Schl�sselprogrammierung falsch"},
{1330, "Zentralsteuerger�t f�r Komfortsystem-J393"}
};

static const error_message_t radio_errors[] PROGMEM = {
/* Fehler Radio/Navi*/
{668, "Bordspannung Klemme 30"},
{849, "S-Kontakt am Z�ndanlassschalter-D"},
{852, "Lautsprecher vorn"},
{853, "Lautsprecher hinten"},
{854, "Ausgang Radiodisplay Schalttafeleinsatz"},
{855, "Verbindung zum CD-Wechsler"},
{856, "Antenne, am Radio"},
{1044, "Steuerger�t falsch codiert"},
{65535, "Steuerger�t defekt"}
};

static const error_message_t sh_errors[] PROGMEM = {
/* Fehler Standheizung*/
{668, "Bordspannung Klemme 30"},
{1407, "Flammenabbruch"},
{1409, "wiederholte Flammenunterbrechung"},
{1410, "Heizger�t �berhitzt"},
{1413, "Dosierpumpe-V54"},
{1414, "Verbrennungsluftgebl�se-V6"},
{1415, "Umw�lzpumpe-V55"},
{1416, "Signal f�r Ansteuerung Frischluftgebl�se"},
{1488, "Geber f�r K�hlmitteltemperatur-Heizung-G241"},
{1489, "Ueberhitzungsf�hler-G189"},
{1490, "Flammw�chter-G64"},
{1491, "Gl�hkerze f�r Heizung-Q9"},
{1492, "Funktionssperre"},
{65535, "Steuerger�t defekt"}
};

static const error_message_t nav_errors[] PROGMEM = {
{481 ,"Steuerger�t f�r Verkehrsfunk keine Kommunikation"},
{625 ,"Geschwindigkeitssignal kein Signal"},
{862 ,"Antenne f�r Navigation (GPS) - R50/R52 Unterbrechung/Kurzschluss nach Plus Kurzschluss nach Masse Leitungsf�hrung defekt"},
{867 ,"Verbindung zum ABS-Steuerger�t kein Signal"},
{1311 ,"Daten-Bus Information kein Signal"},
{65535 ,"Steuerger�t defekt"}
};
/*
static const error_message_t dwa_errors[] PROGMEM = {
	{65535, "Steuerger�t defekt"}
};
*/
static const error_message_t can_errors[] PROGMEM = {
	{778, "Geber f�r Lenkwinkel -G85- keine Kommunikation"},
	{1044, "Steuerger�t falsch codiert"},
	{1312, "Daten-BUS Antrieb defekt."},
	{1314, "Motorsteuerger�t keine Kommunikation."},
	{1315, "Getriebesteuerger�t keine Kommunikation."},
	{1316, "Bremsensteuerger�t keine Kommunikation."},
	{1317, "Steuerger�t im Schalttafeleinsatz -J285- keine Kommunikation."},
	{1321, "Steuerger�t f�r Airbag -J234- keine Kommunikation."},
	{65535, "Steuerger�t defekt"}
};
/*
static const error_message_t lenkrad_errors[] PROGMEM = {
	{65535, "Steuerger�t defekt"}
};
*/
static const error_message_t getriebe_errors[] PROGMEM = {
	{65535, "Steuerger�t defekt"}
};
#endif

