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
const int ledPin = 4;    // LED connected to digital pin 9
float ledValue = 180; // LED is inverted from usual
const int ledMax = 255; // 0 - 255
const int ledMin = 0; // 0 - 255

void setup() {
  // Attach the thrusters and servos to their respective pins
  thrusterL.attach(5,700,2000);
  thrusterR.attach(6,700,2000);
  thrusterV.attach(7,700,2000);
  tiltCamera.attach(8);

  // Set output pin for enabling RS485 transmit
  pinMode(7, OUTPUT);

  Serial.begin(9600); // Initialize serial communication
  
  // Set serial board to low (receive)
  digitalWrite(7, LOW);
  
  thrusterL.write(thrusterNeutral); // Sets all thrusters to their neutral points
  thrusterR.write(thrusterNeutral);
  thrusterV.write(thrusterNeutral);
  tiltCamera.write(90); // Centers the camera vertically - FIGURE OUT WHAT VALUE THIS NEEDS TO BE< AND ADJUST STUFF ACCORDINGLY (MAKE THIS NEUTRAL JOYSTICK, TOO)
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
        // Use x-axis to control pivoting in camera mode
        if (pos1 < 90) {
          pos1 = (pos1 - 90) * cameraRate;
          thrusterL.write(pos1);
          thrusterR.write(-pos1);
        } else if (pos1 > 90) {
          pos1 = (pos1 - 90) * cameraRate;
          thrusterL.write(pos1);
          thrusterR.write(-pos1);
        } else {
          thrusterL.write(90);
          thrusterR.write(90); // MAKE DEFAULT VARIABLE
        }
        
        // Drive camera tilt mechanism
        tiltCamera.write(pos2);

        // Convert vertical value for use as LED control
        pos3 = constrain(pos3, -1, 1);
        
        ledValue += -pos3; // // - is needed because the PWM controller is inverted (255(ish) = min, 0(ish) = max)
        ledValue = constrain(ledValue, ledMin, ledMax);
        analogWrite(ledPin, ledValue);
        delay(10);
        
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
