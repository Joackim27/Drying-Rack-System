#include <LiquidCrystal.h>

// Initialize the LCD library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Define DIP switch pins
const int modeSwitchPin = 6;  // DIP switch 1: Mode switch
const int directionSwitchPin = 7;  // DIP switch 2: Direction control

// Variables to track the current mode
enum Mode {AUTOMATIC, MANUAL};
Mode currentMode = AUTOMATIC;

void setup() {
  // Set up the LCD's number of columns and rows
  lcd.begin(16, 2);
  // Start the serial communication
  Serial.begin(9600);

  // Set up DIP switch pins
  pinMode(modeSwitchPin, INPUT_PULLUP);
  pinMode(directionSwitchPin, INPUT_PULLUP);

  // Print initial message to the LCD
  lcd.print("Waiting for data");
}

void loop() {
  // Read the mode switch DIP switch (DIP switch 1)
  if (digitalRead(modeSwitchPin) == LOW) {
    if (currentMode != MANUAL) {
      currentMode = MANUAL;
      lcd.clear();
      lcd.print("Manual Mode");
      Serial.println("Mode: Manual");
    }
  } else {
    if (currentMode != AUTOMATIC) {
      currentMode = AUTOMATIC;
      lcd.clear();
      lcd.print("Automatic Mode");
      Serial.println("Mode: Automatic");
    }
  }

  if (currentMode == AUTOMATIC) {
    // Check if data is available to read
    if (Serial.available() > 0) {
      // Read the incoming string
      String received = Serial.readStringUntil('\n');
      // Clear the LCD screen
      lcd.clear();
      // Set the cursor to the first row, first column
      lcd.setCursor(0, 0);
      // Print the received string to the LCD
      lcd.print(received);
    }
  } else {
    // Manual mode: check direction control DIP switch (DIP switch 2)
    if (digitalRead(directionSwitchPin) == LOW) {
      // Send manual clockwise command
      Serial.println("Manual Clockwise");
      lcd.setCursor(0, 1);
      lcd.print("Manual CW");
    } else {
      // Send manual counterclockwise command
      Serial.println("Manual Counter Clockwise");
      lcd.setCursor(0, 1);
      lcd.print("Manual CCW");
    }
    delay(500); // Simple debounce
  }
}