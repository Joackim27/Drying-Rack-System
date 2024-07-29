#include <Stepper.h>

const int stepsPerRevolution = 20;  // Number of steps per revolution for your motor
const int motorSpeed = 60;          // Motor speed in RPM
const int runTimeSeconds = 2;       // Run time in seconds
const int rainSensorPin = 2;        // Rain sensor connected to pin 2
const int ldrPin = A0;              // LDR sensor connected to analog pin A0
const int relayPin = 13;            // Relay connected to pin 13

Stepper myStepper(stepsPerRevolution, A1, A2, A3, A4);  // Pins connected to IN1, IN2, IN3, IN4 on L293D

int lastRainState = -1; // Initial state of rain sensor
int lastAction = -1; // -1: no action, 0: counter-clockwise, 1: clockwise

void setup() {
  Serial.begin(9600);
  pinMode(rainSensorPin, INPUT);  // Set rain sensor pin as input
  pinMode(ldrPin, INPUT);         // Set LDR pin as input
  pinMode(relayPin, OUTPUT);      // Set relay pin as output
  digitalWrite(relayPin, LOW);    // Ensure relay is initially off
  myStepper.setSpeed(motorSpeed); // Set the speed at 60 rpm
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();

    if (command == 'C') {
      // Rotate stepper motor clockwise for 2 seconds
      int steps = (motorSpeed * stepsPerRevolution / 60) * runTimeSeconds;
      myStepper.step(steps);
      lastAction = 1;
      digitalWrite(relayPin, LOW);  // Turn off relay
    } else if (command == 'A') {
      // Rotate stepper motor counter-clockwise for 2 seconds
      int steps = (motorSpeed * stepsPerRevolution / 60) * runTimeSeconds;
      myStepper.step(-steps);
      lastAction = 0;
      digitalWrite(relayPin, HIGH); // Turn on relay
    } else if (command == 'R') {
      // Send rain sensor state to Arduino 2
      int rain = digitalRead(rainSensorPin);
      Serial.print("R:");
      Serial.println(rain);
    } else if (command == 'L') {
      // Send only LDR value to Arduino 2
      int ldrValue = analogRead(ldrPin);
      Serial.print("L:");
      Serial.println(ldrValue);
    }
  }

  // Continuously send LDR value to Arduino 2
  int ldrValue = analogRead(ldrPin);
  Serial.print("L:");
  Serial.println(ldrValue);
  delay(500); // Delay to prevent flooding the serial communication
}
