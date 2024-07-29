#include <LiquidCrystal.h>

// LCD pin connections: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int buttonClockwise = 6;
const int buttonCounterClockwise = 7;
const int dipSwitchPin = 8;  // DIP switch connected to pin 8

bool lastDipSwitchState = HIGH;  // Initial state, assuming DIP switch is off
int lastRainState = -1;  // Initial state of rain sensor, undefined
unsigned long lastMillis = 0;

void setup() {
  pinMode(buttonClockwise, INPUT_PULLUP);  // Set pin 6 as input with internal pull-up resistor
  pinMode(buttonCounterClockwise, INPUT_PULLUP);  // Set pin 7 as input with internal pull-up resistor
  pinMode(dipSwitchPin, INPUT_PULLUP);  // Set pin 8 as input with internal pull-up resistor
  lcd.begin(20, 4);  // Initialize the LCD with 20 columns and 4 rows
  lcd.print("DRYING RACK SYSTEM");
  Serial.begin(9600);
}

void loop() {
  // Check if buttonClockwise is pressed
  if (digitalRead(buttonClockwise) == LOW) {
    // Send command to rotate clockwise
    Serial.write('C');
    delay(200);  // Debounce delay
  }
  
  // Check if buttonCounterClockwise is pressed
  if (digitalRead(buttonCounterClockwise) == LOW) {
    // Send command to rotate counter-clockwise
    Serial.write('A');
    delay(200);  // Debounce delay
  }

  bool currentDipSwitchState = digitalRead(dipSwitchPin);
  unsigned long currentMillis = millis();

  if (currentDipSwitchState == LOW && currentMillis - lastMillis >= 1000) {
    // DIP switch is on and 1 second has passed
    lastMillis = currentMillis;
    Serial.write('R'); // Request rain sensor state
    Serial.write('L'); // Request LDR value
  }

  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');

    // Parse the received data
    if (data.startsWith("L:")) {
      int ldrValue = data.substring(2).toInt();

      // Display LDR readings on the third line of the LCD
      lcd.setCursor(0, 2);
      lcd.print("LDR: ");
      lcd.print(ldrValue);
      lcd.print("     ");  // Ensure to clear any previous longer value
    } else if (data.startsWith("R:")) {
      int rainState = data.substring(2).toInt();

      // Display "RAINING" or "NOT RAINING" on the second line of the LCD
      lcd.setCursor(0, 1);
      if (rainState == 1) {
        lcd.print("RAINING        ");
      } else {
        lcd.print("NOT RAINING    ");
      }

      // Check DIP switch state and perform actions based on rain state
      if (currentDipSwitchState == LOW) {
        if (rainState != lastRainState) {
          lcd.setCursor(0, 3);
          if (rainState == 1) {
            Serial.write('A');  // Rotate counter-clockwise if rain is detected
            lcd.print("CCW                  ");
            delay(2000);
            lcd.setCursor(0, 3);
            lcd.print("ACTION COMPLETE ");
          } else {
            Serial.write('C');  // Rotate clockwise if no rain
            lcd.print("CW                   ");
            delay(2000);
            lcd.setCursor(0, 3);
            lcd.print("ACTION COMPLETE ");
          }
          lastRainState = rainState;
        }
      }
    }
  } else if (currentDipSwitchState == HIGH) {
    // Clear the LCD lines 1, 2, and 3 if DIP switch is off
    lcd.setCursor(0, 1);
    lcd.print("MANUAL MODE");
    lcd.setCursor(0, 3);
    lcd.print("                    ");
  }

  lastDipSwitchState = currentDipSwitchState;
}
