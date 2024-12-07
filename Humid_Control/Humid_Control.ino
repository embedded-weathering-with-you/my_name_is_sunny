/*--------------------------------------------
Module Name : Humid_Control.ino

REVISION HISTORY in GitHub:
URL : https://github.com/embedded-weathering-with-you/today_is_sunny

Description : 
This code is for a smart Teru Teru Bozu bot.
It uses a DHT11 sensor, a stepper motor, and a MAX7219 LED matrix.
--------------------------------------------*/

// Include necessary libraries
#include "DHT.h"
#include <LedControl.h>

// DHT11 Sensor Configuration
#define DHTPIN 6        // DHT sensor pin
#define DHTTYPE DHT11   // DHT11 sensor type

// Stepper Motor Pin Configuration
const int motorPin1 = 5;    // IN1
const int motorPin2 = 4;    // IN2
const int motorPin3 = 3;    // IN3
const int motorPin4 = 2;    // IN4

// MAX7219 Matrix LED Pin Configuration
#define DIN 11
#define CLK 13
#define CS 10

LedControl lc = LedControl(DIN, CLK, CS, 1);

// Stepper Motor Steps Configuration
const int steps[] = {B1000, B1100, B0100, B0110, B0010, B0011, B0001, B1001, B0000};

// Stepper Motor Variables
const int stepsPerRevolution = 2048;  // Steps for one full revolution
const int stepsFor180 = stepsPerRevolution / 2;  // Steps for 180 degrees
int motorSpeed = 1000;    // Delay between steps (microseconds)
bool isAt180 = false;     // Tracks if motor is at 180 degrees
bool reverseDirection = false;  // Tracks motor direction

DHT dht(DHTPIN, DHTTYPE);  // Initialize DHT object

void setup() {
  // Set motor pins as output
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);

  // Initialize LED Matrix
  lc.shutdown(0, false);   // Turn on LED matrix
  lc.setIntensity(0, 8);   // Set brightness (0-15)
  lc.clearDisplay(0);      // Clear display

  Serial.begin(9600);
  Serial.println("DHT11, Stepper Motor, and Matrix LED Test");
  dht.begin();
} // End of setup

void loop() {
  // Read humidity
  float h = dht.readHumidity();

  // Check if sensor data is valid
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Print humidity
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" %");

  // Rotate to 180 degrees if humidity > 60%
  if (h > 60 && !isAt180) {
    Serial.println("Motor: Rotating to 180 degrees");
    rotateTo180();
    isAt180 = true;
    displaySmileyFace(); // Show smiley face on LED
  }
  // Return to 0 degrees if humidity <= 60%
  else if (h <= 60 && isAt180) {
    Serial.println("Motor: Returning to 0 degrees");
    returnToZero();
    isAt180 = false;
    lc.clearDisplay(0); // Clear LED display
  }

  delay(2000); // Read sensor data every 2 seconds
}

// Rotate to 180 degrees
void rotateTo180() {
  if (!reverseDirection) {
    for (int i = 0; i < stepsFor180; i++) {
      clockwise();
    }
  } else {
    for (int i = 0; i < stepsFor180; i++) {
      counterClockwise();
    }
  }
  reverseDirection = !reverseDirection;  // Reverse direction
}

// Return to 0 degrees
void returnToZero() {
  if (!reverseDirection) {
    for (int i = 0; i < stepsFor180; i++) {
      clockwise();
    }
  } else {
    for (int i = 0; i < stepsFor180; i++) {
      counterClockwise();
    }
  }
  reverseDirection = !reverseDirection;  // Reverse direction
}

// Rotate one step clockwise
void clockwise() {
  for (int i = 7; i >= 0; i--) {
    motorSignalOutput(i);
    delayMicroseconds(motorSpeed);
  }
}

// Rotate one step counterclockwise
void counterClockwise() {
  for (int i = 0; i < 8; i++) {
    motorSignalOutput(i);
    delayMicroseconds(motorSpeed);
  }
}

// Output motor signals
void motorSignalOutput(int out) {
  digitalWrite(motorPin1, bitRead(steps[out], 0));
  digitalWrite(motorPin2, bitRead(steps[out], 1));
  digitalWrite(motorPin3, bitRead(steps[out], 2));
  digitalWrite(motorPin4, bitRead(steps[out], 3));
}

// Display smiley face on LED Matrix
void displaySmileyFace() {
  // Define smiley face pattern
  byte smileyFace[8] = {
    B00000000, //                
    B01100110, //  **  **  (eyes)
    B10011001, // *  **  * (eyes)
    B00000000, //                
    B00000000, //                
    B00000000, //   
    B00000000, //                
    B00000000  //                
  };

  // Output pattern to LED Matrix
  for (int i = 0; i < 8; i++) {
    lc.setRow(0, i, smileyFace[i]);
  }
}
