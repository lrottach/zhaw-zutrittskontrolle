# Zutrittskontrolle (RFID und PIN)

## Projektübersicht

Dieses Projekt realisiert ein Zutrittskontrollsystem mit folgenden Hauptfunktionen:
- **RFID-Technologie**: Zugang mittels autorisierter RFID-Karten oder Badges.
- **PIN-Eingabe**: Alternativer Zugang über ein PIN-Tastenfeld.
- **LED-Anzeige**: Grüne LED zeigt erfolgreiche Authentifizierung, rote LED signalisiert Fehler.
- **Alarmfunktion**: Alarm wird nach drei fehlerhaften Versuchen ausgelöst, um Manipulationen zu erkennen.
- **LCD-Display**: Ausgabe von Status, Informationen und Fehlermeldungen
- **Hardwarekomponenten**: Aufbau mit Arduino-Mikrocontroller, RFID-Modul, Tastenfeld, LEDs, Summer, LCD-Display und Servo-Motor.

### Zielsetzung
Ziel ist die Entwicklung eines flexiblen und sicheren Systems, das sich leicht installieren und verwenden lässt. Es bietet zwei Zugangsmethoden und schützt vor unberechtigtem Zugang durch eine integrierte Alarmfunktion.

## Ordnerstruktur

```
/project
├── /.github/workflows
│ └── test-build.yml   # GitHub Actions test builds
├── /src
│ └── project.ino  # Hauptcode für den Arduino
├── /docs
│ └── schaltplan.png # Schaltplan für den Aufbau
├── /assets │
└── README.md # Diese Datei
```

## Installation und Einrichtung

### Hardwareanforderungen
1. **Arduino Mega 2560**
2. **RFID-Modul RC522**
3. **4x4 Matrix Keypad**
4. **LEDs (grün/rot)**
5. **Summer**
6. **Servo-Motor**
7. **LCD-Display (Modul I2C 20 x 4 LCD)**
8. **Verkabelungsmaterial (Breadboard, Jumper-Kabel)**

### Softwareanforderungen
- **Arduino IDE**: Version 1.8.19 oder höher
- Bibliotheken:
  - `#include <Servo.h>` Steuerung des Servo-Motors
  - `#include <Wire.h>` I2C-Kommunikation
  - `#include <LiquidCrystal_I2C.h>` Steuerung von LCD-Displays über die I2C-Schnittstelle
  - `#include <MFRC522.h>` Kommunikation mit dem RFID-Modul RC522
  - `#include <Keypad.h>` Steuerung von Matrix-Tastenfeldern

### Einrichtungsschritte
1. Lade die Arduino IDE herunter und installiere die oben genannten Bibliotheken.
2. Schliesse die Hardware gemäss dem bereitgestellten Schaltplan an.
3. Öffne die Datei `project.ino` in der Arduino IDE.
4. Lade den Sketch auf den Arduino hoch.

## Kontakt
Dieses Projekt wurde im Rahmen des Moduls **Hardwarenahe Programmierung** am **CAS Computer Science 1** der ZHAW erstellt.

## Projekt Team
Siehe Konzept.

