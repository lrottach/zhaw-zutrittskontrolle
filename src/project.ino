// === Bibliotheken ===
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <Keypad.h>

/*
Projekt: Zutrittskontrollsystem mit RFID und PIN
Autor: Project Team
Datum: 21.01.2025
Beschreibung:
    Dieses Projekt implementiert ein Zutrittskontrollsystem mit mehreren Sicherheitsfunktionen:
    - Authentifizierung durch RFID und PIN
    - Zugangsanzeige über LEDs und ein LCD-Display
    - Alarm bei wiederholten Fehlversuchen

Hardware:
    - siehe Readme.md

Bibliotheken:
    - LiquidCrystal_I2C.h: Ansteuerung des LCD-Displays
    - Wire.h: I2C-Kommunikation
    - MFRC522.h: RFID-Lesemodul
    - Keypad.h: Tastenfeld
*/

// === Pin-Definitionen ===
#define RST_PIN 9
#define SS_PIN 10

// === Hardware-Setup ===
Servo myServo; // Servo setup
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C LCD Setup (Adresse 0x27, 16x2 Display)
MFRC522 mfrc522(SS_PIN, RST_PIN); // RFID setup

// === Keypad-Konfiguration ===
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {30, 32, 34, 36};
byte colPins[COLS] = {38, 40, 42, 44};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// === Konstanten und Variablen ===
const int redLED = 4; // Rote LED
const int greenLED = 5; // Grüne LED
const int buzzer = 6;   // Buzzer
const String PIN_CODE = "1234"; // Vordefinierter PIN-Code

const String authorizedRFIDs[] = {"E36AFDFB", "A1B2C3D4", "33603030"}; // Zugelassene RFID-UIDs
const String usernames[] = {"Alice", "Bob", "Charlie"}; // Benutzernamen
const int numRFIDs = sizeof(authorizedRFIDs) / sizeof(authorizedRFIDs[0]);

String enteredPIN = ""; // Eingegebener PIN-Code
int failedAttempts = 0; // Zähler für fehlgeschlagene Versuche

// === Funktionsprototypen ===
int getUserIndex(String rfidUID);
void welcomeUser(int userIndex);
void grantAccess();
void denyAccess();
void triggerAlarm();

// === Initialisierung der Hardware ===
void setup() {
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  myServo.attach(3);
  myServo.write(0); // Tür verriegeln

  lcd.init(); // LCD initialisieren
  lcd.backlight(); // Hintergrundbeleuchtung einschalten
  lcd.setCursor(0, 1);
  lcd.print("System Ready");

  Serial.begin(9600);

  SPI.begin();
  mfrc522.PCD_Init();

  Serial.println("System initialized. Waiting for input...");
  delay(2000);
  lcd.clear();
}

void loop() {
  // === Standardanzeige ===
  lcd.setCursor(0, 1);
  lcd.print("Scan RFID or");
  lcd.setCursor(0, 2);
  lcd.print("Enter PIN:");

  // === RFID-Überprüfung ===
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String rfidUID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      rfidUID += String(mfrc522.uid.uidByte[i], HEX);
    }
    rfidUID.toUpperCase();
    mfrc522.PICC_HaltA();

    Serial.print("RFID UID detected: ");
    Serial.println(rfidUID);

    int userIndex = getUserIndex(rfidUID);
    if (userIndex != -1) {
      Serial.println("Authorized RFID card. Access granted.");
      welcomeUser(userIndex);
      grantAccess();
    } else {
      Serial.println("Unauthorized RFID card. Access denied.");
      denyAccess();
    }
  }

  // === PIN-Überprüfung ===
  char key = keypad.getKey();
  if (key) {
    Serial.print("Keypad input detected: ");
    Serial.println(key);

    if (key == '*') {
      enteredPIN = ""; // Clear PIN
      Serial.println("PIN reset. Current PIN cleared.");
    } else if (key == '#') {
      Serial.print("PIN entered: ");
      Serial.println(enteredPIN);

      if (enteredPIN == PIN_CODE) {
        Serial.println("Correct PIN. Access granted.");
        grantAccess();
      } else {
        Serial.println("Incorrect PIN. Access denied.");
        denyAccess();
      }
      enteredPIN = ""; // Reset PIN after submission
    } else {
      enteredPIN += key; // Append key to entered PIN
    }
  }
}

// === Funktionen ===

// UID prüfen und Index des Benutzers zurückgeben
int getUserIndex(String rfidUID) {
  for (int i = 0; i < numRFIDs; i++) {
    if (authorizedRFIDs[i] == rfidUID) {
      return i; // Return index of authorized user
    }
  }
  return -1; // Not found
}

// Begrüssungsfunktion auf LCD
void welcomeUser(int userIndex) {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Welcome, ");
  lcd.print(usernames[userIndex]);
  delay(2000);
  lcd.clear();
}

// Zugang gewähren
void grantAccess() {
  Serial.println("Access granted. Unlocking...");
  digitalWrite(greenLED, HIGH);
  lcd.clear(); // Löscht das Display
  lcd.setCursor(0, 1);
  lcd.print("Access Granted");

  myServo.write(90); // Unlock door
  delay(5000); // Tür bleibt 5 Sekunden offen

  myServo.write(0); // Lock door again
  Serial.println("Door locked.");
  digitalWrite(greenLED, LOW);
  lcd.clear();

  failedAttempts = 0; // Reset failed attempts
}

// Zugang verweigern
void denyAccess() {
  failedAttempts++;
  Serial.println("Access denied. Turning on red LED...");
  digitalWrite(redLED, HIGH);
  lcd.clear(); // Löscht das Display
  lcd.setCursor(0, 1);
  lcd.print("Access Denied");

  delay(2000);

  digitalWrite(redLED, LOW);
  Serial.println("Red LED OFF.");
  lcd.clear();

  // Falls mehr als 3 Fehlversuche -> Alarm auslösen
  if (failedAttempts >= 3) {
    Serial.println("Too many failed attempts. Triggering alarm...");
    triggerAlarm();
  }
}

// Alarm auslösen
void triggerAlarm() {
  Serial.println("ALARM triggered! Activating buzzer...");
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("ALARM TRIGGERED!");

  for (int i = 0; i < 5; i++) {
    Serial.print("Alarm sound ");
    Serial.println(i + 1);
    digitalWrite(buzzer, HIGH);
    delay(200);
    digitalWrite(buzzer, LOW);
    delay(200);
  }

  Serial.println("Alarm ended. Resetting failed attempts.");
  lcd.clear();
  failedAttempts = 0; // Reset failed attempts
}
