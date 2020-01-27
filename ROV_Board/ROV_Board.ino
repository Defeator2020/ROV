#include <Servo.h>

const int ledPin = 13; // The pin that the LED is attached to

// Create servo objects to control everything (servos + ESCs)
Servo thrusterL;
Servo thrusterR;
Servo thrusterV;
Servo panCamera;
Servo tiltCamera;

// Create variables to store the command data
int pos1 = 90;
int pos2 = 90;
int pos3 = 90;
int peripheralControl = 0;

void setup() {
  // Attach the thrusters and servos to their respective pins
  thrusterL.attach(5);
  thrusterR.attach(6);
  thrusterV.attach(7);
  panCamera.attach(9);
  tiltCamera.attach(10);

  // Perform ESC handshake(s) - CURRENTLY ONLY LEFT THRUSTER (PIN 5)
  thrusterL.write(140);
  delay(4000);
  thrusterL.write(40);
  delay(4000);
  
  Serial.begin(9600); // Initialize serial communication
  pinMode(ledPin, OUTPUT); // Initialize the LED pin as an output
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
      // Constrain the thruster/servo values to 0 - 180 (sanity check on values / erroneous command protection)
      pos1 = constrain(pos1, 0, 180);
      pos2 = constrain(pos2, 0, 180);
      pos3 = constrain(pos3, 0, 180);
  
      if (peripheralControl == 0) {
        // Write the servo position(s) to the servo(s)
        thrusterL.write(pos1);
        thrusterR.write(pos2);
        thrusterV.write(pos3);
      } else if (peripheralControl == 1) {
        panCamera.write(pos1);
        tiltCamera.write(pos2);
      } else if (peripheralControl == 2) {
    
      } else {
    
      }
    }
  }
}
