# Anwenderdokumentation NFCWriter MillControl

## Zweck
MillControl steuert die Kaffeemuehle und bucht Kaffeezuege von NFC-Karten ab.
Die Logdaten werden lokal auf MillControl gespeichert und koennen ueber das Terminal/GUI abgerufen oder zurueckgesetzt werden.

## Alltag: Kaffee beziehen
1. NFC-Karte vorhalten.
2. Mit den Tasten den gewuenschten Bezug waehlen.
3. Bei ausreichendem Guthaben wird der Bezug freigegeben und der Kartenstand reduziert.

Hinweis:
- Bei zu wenig Guthaben erscheint ein Low-Credit-Zustand.
- Schreibfehler werden intern mehrfach wiederholt, bevor abgebrochen wird.

## Tastenlogik (Kurzueberblick)
Die genaue Zustandsmaschine steht in [state_machine.md](state_machine.md).

Typische Zuordnung:
- Linke Taste: Einzelbezug / linksseitige Optionen
- Rechte Taste: Doppelbezug / rechtsseitige Optionen
- Beide Tasten: Ruecksprung, Anzeigewechsel oder Speichern je nach Zustand

## Mill-Logs
MillControl erfasst pro Karte aggregierte Werte:
- Abbuchungen
- Rueckerstattungen
- Letzter bekannter Kredit

Diese Daten stellt MillControl ueber sein lokales WLAN-API bereit.

## Mill-Logs zuruecksetzen
Es gibt zwei Wege:
1. Ueber die GUI: Analyse -> Mill-Logs anzeigen -> Zuruecksetzen
2. Ueber Terminal-Firmware: `ResetMillLogs`

Beim erfolgreichen Reset werden die MillControl-Logdateien geloescht.

## Netzwerkanbindung
Standardwerte:
- SSID: `MillControl`
- Passwort: `mill1234`
- API-Key: `mill_secret_key`

Der API-Key ist fuer HTTP-Authentifizierung (Bearer) und nicht identisch mit dem NFC-Tag-Passwort.

## NFC-Tag-Passwort
Das Tag-Passwort (NTAG-PWD) wird in Firmware-Defines gehalten und muss zwischen Terminal und MillControl synchron sein.

Aktuell (Stand Code):
- PWD: `FF AB BA FF`
- PACK: `0E 05`

Wenn geaendert:
1. In beiden Projekten identisch aendern.
2. Beide Firmwares neu flashen.
3. Tags bei Bedarf neu initialisieren/migrieren.

## Fehlerbilder
- Keine Log-Antwort: WLAN/API-Key pruefen.
- Logs bleiben trotz Reset: pruefen, ob Terminal und MillControl auf passender Firmware sind.
- Karte nicht lesbar: Tag-Auth (PWD/PACK) und Tag-Initialisierung pruefen.
