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

// Create variables to manage the LED conditions and logic
const int ledPin = 4;    // LED connected to digital pin 9
float ledValue = 180; // LED is inverted from usual
const int ledMax = 255; // 0 - 255 (WRITE COMMAND TO USE SERIAL TO FIND ABSOLUTE MAX / MIN VALUES FOR THIS)
const int ledMin = 0; // 0 - 255 (")

void setup() {
  // Attach the thrusters and servos to their respective pins
  thrusterL.attach(5);
  thrusterR.attach(6);
  thrusterV.attach(7);
  tiltCamera.attach(8);

  // Perform ESC handshake(s) - CURRENTLY ONLY LEFT THRUSTER (PIN 5)
  thrusterL.write(140);
  delay(4000);
  thrusterL.write(40);
  delay(4000);
  
  Serial.begin(9600); // Initialize serial communication
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
        tiltCamera.write(pos2);
        
      } else if (peripheralControl == 2) {
        pos1 = constrain(pos1,-1,1); // WILL CHANGING THIS VALUE (pos1) HERE MESS ANYTHING UP? DO I NEED TO MAKE A NEW VARIABLE JUST FOR THIS CALCULATION?
        ledValue += pos1 * -1; // -1 is needed because the PWM controller is inverted (255(ish) = min, 0(ish) = max)
        ledValue = constrain(ledValue, ledMin, ledMax);

        analogWrite(ledPin, ledValue);
        delay(10);
        
      } else { // TEMPORARY ERROR STATE - NO THRUSTERS, CAMERA CENTERED
        thrusterL.write(90); // VERIFY THAT THIS REALLY IS 0% (NEUTRAL) THROTTLE
        thrusterR.write(90);
        thrusterV.write(90);
        tiltCamera.write(90);
      }
    }
  }
}
