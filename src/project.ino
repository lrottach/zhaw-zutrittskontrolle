#include <Servo.h>
#include <LiquidCrystal.h>
#include <MFRC522.h>
#include <Keypad.h>

// Pins for RFID
#define RST_PIN 9
#define SS_PIN 10

// Servo setup
Servo myServo;

// LCD Pins (non-I2C)
// Initialisiere LCD mit den Pins (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(41, 39, 37, 35, 33, 31);

// RFID setup
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Keypad setup
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

// Constants
const innt redLED = 4;
const int greenLED = 5;
const int buzzer = 6;
const String PIN_CODE = "1234";

// Authorized RFID UIDs
const String authorizedRFIDs[] = {"E36AFDFB", "A1B2C3D4", "33603030"};
const int numRFIDs = sizeof(authorizedRFIDs) / sizeof(authorizedRFIDs[0]);

// Variables
String enteredPIN = "";
int failedAttempts = 0;

void setup() {
  // Initialize components
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  myServo.attach(3);
  myServo.write(0); // Servo in locked position

  lcd.begin(16, 2);
  Serial.begin(9600);

  SPI.begin();
  mfrc522.PCD_Init();

  Serial.println("System initialized. Waiting for input...");
  lcd.print("System Ready");
  delay(2000);
  lcd.clear();
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("Scan RFID or");
  lcd.setCursor(0, 1);
  lcd.print("Enter PIN:");

  // Check RFID
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String rfidUID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      rfidUID += String(mfrc522.uid.uidByte[i], HEX);
    }
    rfidUID.toUpperCase();
    mfrc522.PICC_HaltA();

    Serial.print("RFID UID detected: ");
    Serial.println(rfidUID);

    if (isAuthorizedRFID(rfidUID)) {
      Serial.println("Authorized RFID card. Access granted.");
      grantAccess();
    } else {
      Serial.println("Unauthorized RFID card. Access denied.");
      denyAccess();
    }
  }

  // Check PIN
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
      enteredPIN += key;
      Serial.print("Current PIN: ");
      Serial.println(enteredPIN);

      lcd.setCursor(0, 1);
      lcd.print("PIN: ");
      lcd.print(enteredPIN);
    }
  }
}

bool isAuthorizedRFID(String rfid) {
  for (int i = 0; i < numRFIDs; i++) {
    if (rfid == authorizedRFIDs[i]) {
      return true;
    }
  }
  return false;
}

void grantAccess() {
  Serial.println("Access granted. Unlocking...");
  digitalWrite(greenLED, HIGH);
  lcd.clear();
  lcd.print("Access Granted");
  Serial.println("Green LED ON. Door unlocked.");

  myServo.write(90); // Unlock door
  delay(10000);

  myServo.write(0); // Lock door again
  Serial.println("Door locked.");
  digitalWrite(greenLED, LOW);
  lcd.clear();

  failedAttempts = 0; // Reset failed attempts
}

void denyAccess() {
  failedAttempts++;
  Serial.println("Access denied. Turning on red LED...");
  digitalWrite(redLED, HIGH);
  lcd.clear();
  lcd.print("Access Denied");

  delay(2000);

  digitalWrite(redLED, LOW);
  Serial.println("Red LED OFF.");
  lcd.clear();

  if (failedAttempts >= 3) {
    Serial.println("Too many failed attempts. Triggering alarm...");
    triggerAlarm();
  }
}

void triggerAlarm() {
  Serial.println("ALARM triggered! Activating buzzer...");
  lcd.clear();
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
