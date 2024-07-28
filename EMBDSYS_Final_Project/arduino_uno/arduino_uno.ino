#include <Stepper.h>

// Define the number of steps per revolution for your stepper motor
const int stepsPerRevolution = 20;

// Initialize the stepper library on pins A1, A2, A3, A4
Stepper stepperA(stepsPerRevolution, A1, A2, A3, A4);

// Define the sensor pins
const int rainSensorPin = 2;
const int ldrPin = A0;
const int relayPin = 13; // Relay connected to digital pin 13

// Variables to manage timing and state
const long interval = 5000; // 5 seconds
const int lightThreshold = 500; // Adjust this threshold based on your LDR setup

// Variables to track the previous state
bool previousIsRaining = false;
bool previousGoodLight = false;

// Variable to track the last action
enum Action {NONE, CLOCKWISE, COUNTERCLOCKWISE};
Action lastAction = NONE;

// Variable to track the current mode
enum Mode {AUTOMATIC, MANUAL};
Mode currentMode = AUTOMATIC;

void setup() {
  // Set up the rain sensor pin
  pinMode(rainSensorPin, INPUT);

  // Set up the stepper motor control pins
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);

  // Set up the relay pin
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Ensure relay is off initially

  // Set the speed of the stepper motor
  stepperA.setSpeed(60);

  // Start the serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  // Check if data is available to read
  if (Serial.available() > 0) {
    // Read the incoming string
    String received = Serial.readStringUntil('\n');
    if (received.startsWith("Mode:")) {
      // Change mode
      if (received.endsWith("Manual")) {
        currentMode = MANUAL;
        Serial.println("Switched to Manual Mode"); // Debugging
      } else if (received.endsWith("Automatic")) {
        currentMode = AUTOMATIC;
        Serial.println("Switched to Automatic Mode"); // Debugging
      }
    } else if (currentMode == MANUAL) {
      // Handle manual commands
      if (received == "Manual Clockwise") {
        Serial.println("Received Manual Clockwise Command"); // Debugging
        rotateClockwise();
      } else if (received == "Manual Counter Clockwise") {
        Serial.println("Received Manual Counter Clockwise Command"); // Debugging
        rotateCounterClockwise();
      }
    }
  }

  if (currentMode == AUTOMATIC) {
    // Read the rain sensor
    int rain = digitalRead(rainSensorPin);

    // Read the LDR sensor
    int lightLevel = analogRead(ldrPin);

    // Determine if it is raining
    bool isRaining = (rain == HIGH);

    // Determine if it is good sunlight
    bool goodLight = (lightLevel > lightThreshold);

    // Determine the current action based on the sensor readings
    Action currentAction;
    if (isRaining) {
      currentAction = COUNTERCLOCKWISE;
    } else if (!isRaining && !goodLight) {
      currentAction = COUNTERCLOCKWISE;
    } else {
      currentAction = CLOCKWISE;
    }

    // Check if the sensor state or action has changed
    if (isRaining != previousIsRaining || goodLight != previousGoodLight || currentAction != lastAction) {
      previousIsRaining = isRaining;
      previousGoodLight = goodLight;
      lastAction = currentAction;

      // Send status over serial
      if (currentAction == COUNTERCLOCKWISE) {
        Serial.println("Counter Clockwise Action");
      } else if (currentAction == CLOCKWISE) {
        Serial.println("Clockwise Action");
      }

      // Run the motor based on the current state for 5 seconds
      unsigned long startMillis = millis();
      while (millis() - startMillis < interval) {
        if (currentAction == COUNTERCLOCKWISE) {
          rotateCounterClockwise();
        } else if (currentAction == CLOCKWISE) {
          rotateClockwise();
        }
      }

      // Stop the motor after 5 seconds
      stepperA.step(0); // Ensure the motor stops

      // Activate the relay if it is raining
      if (isRaining) {
        digitalWrite(relayPin, HIGH); // Activate relay to deploy cover
      } else {
        digitalWrite(relayPin, LOW); // Deactivate relay to retract cover
      }
    }

    // Add a small delay before the next reading
    delay(1000);
  }
}

void rotateClockwise() {
  Serial.println("Rotating Clockwise"); // Debugging
  stepperA.step(stepsPerRevolution); // Rotate clockwise
  delay(100); // Adjust delay for smooth rotation
}

void rotateCounterClockwise() {
  Serial.println("Rotating Counter Clockwise"); // Debugging
  stepperA.step(-stepsPerRevolution); // Rotate counterclockwise
  delay(100); // Adjust delay for smooth rotation
}