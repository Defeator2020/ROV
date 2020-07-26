#include <Wire.h>
#include <Servo.h>

// Create servo objects to control everything (servos + ESCs)
Servo thrusterL;
Servo thrusterR;
Servo thrusterV;
Servo tiltCamera;

// Create variables to store the command data
int pos1 = 90;
int pos2 = 90;
int pos3 = 90;
int peripheralControl = 0;

float cameraRate = .125; // Rate adjust for thruster commands while in camera mode, in percent (0 - 1).
int thrusterNeutral = 90; // Identifies the neutral point for all thrusters, and will return to this value if anything erroneous happens

// Create variables to manage the LED conditions and logic
const int ledPin = 3;    // LED control transistor is connected to digital pin 3
int ledChangeRate = 1; // Dictates the rate at which the lateral joystick will shift the LED brightness when in camera mode
float ledValue = 180; // LED is inverted from usual
const int ledMax = 200; // 0 - 255
const int ledMin = 0; // 0 - 255

void setup() {
  // Attach the thrusters and servos to their respective pins
  thrusterL.attach(5,700,2000);
  thrusterR.attach(6,700,2000);
  thrusterV.attach(7,700,2000);
  tiltCamera.attach(9);

  // Set output pin for enabling RS485 transmit and laser control (respectively)
  pinMode(3, OUTPUT);
  pinMode(12, OUTPUT);

  Serial.begin(9600); // Initialize serial communication
  
  // Set serial board to low (receive)and disengage lasers
  digitalWrite(12, LOW);
  
  thrusterL.write(thrusterNeutral); // Sets all thrusters (and the camera) to their neutral points
  thrusterR.write(thrusterNeutral);
  thrusterV.write(thrusterNeutral);
  tiltCamera.write(80);
  analogWrite(ledPin, ledValue);
}

void loop() {
  // If there's any serial available, read it
  while (Serial.available() > 0) {
    int pos1 = Serial.parseInt(); // Look for the next valid integer in the incoming serial stream
    int pos2 = Serial.parseInt(); // Do it again
    int pos3 = Serial.parseInt(); // Do it again
    int peripheralControl = Serial.parseInt(); // Do it again

    // Look for the newline. That marks the end of the sentence
    if (Serial.read() == '\n') {
      if (peripheralControl == 0) {
        // Write the servo position(s) to the servo(s)
        thrusterL.write(pos1);
        thrusterR.write(pos2);
        thrusterV.write(pos3);
        
      } else if (peripheralControl == 1) {
        // Use vertical joystick to control camera
        if (pos2 < 85 or pos2 > 95) {
          pos2 -= 10; // Lower resting value to level camera
          pos2 = constrain(pos2, 55, 115); // Constrain camera values to available tilt range - MAYBE SPREAD OUT TO TAKE UP FULL JOYSTICK RANGE AT SOME POINT?
        } else {
          pos2 = 80;
        }
        
        // Drive camera tilt mechanism
        tiltCamera.write(pos2);

        // Ensures that 'pos3' commands are within expected range (-1, 1, or 0) - RESTRICT TO ONLY THOSE VALUES?
        pos3 = constrain(pos3, -1, 1); // IS THIS EVEN NECESSARY? Probably not, but it can't really hurt.

        // Use lateral joystick to control the LED brightness
        if (pos1 > 100) {
          ledValue += ledChangeRate;
        } else if (pos1 < 80) {
          ledValue -= ledChangeRate;
        }
        
        // Update written LED value as was updated above
        ledValue = constrain(ledValue, ledMin, ledMax);
        analogWrite(ledPin, ledValue);

        // Engage lasers on Nunchuck "c" key press
        if (pos3 == 1) {
          digitalWrite(12, HIGH);
        } else {
          digitalWrite(12, LOW);
        } 
        
        // Write neutral values to the thrusters in order to prevent any... weirdness
        thrusterL.write(thrusterNeutral);
        thrusterR.write(thrusterNeutral);
        
      } else if (peripheralControl == 2) {
        // PUT CODE FOR MANIPULATOR / SAMPLING INSTRUMENTS HERE
        
      } else { // Error state - zero all thrusters
        thrusterL.write(thrusterNeutral);
        thrusterR.write(thrusterNeutral);
        thrusterV.write(thrusterNeutral);
      }
    } else { // Error state - zero all thrusters
      thrusterL.write(thrusterNeutral);
      thrusterR.write(thrusterNeutral);
      thrusterV.write(thrusterNeutral);
    }
  }
}
