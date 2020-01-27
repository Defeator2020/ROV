#include <Servo.h>

const int ledPin = 13; // the pin that the LED is attached to
int incomingByte; // a variable to read incoming serial data into

// create servo objects to control the servos
Servo servo1;
Servo servo2;

// create variables to store the servo positions
int servo1pos = 90;
int servo2pos = 90;

void setup() {
  // attaches the servo on pin 9 to the servo object
  servo1.attach(9);
  servo2.attach(10);
  
  Serial.begin(9600); // initialize serial communication
  pinMode(ledPin, OUTPUT); // initialize the LED pin as an output
}

void loop() {
  // if there's any serial available, read it
  while (Serial.available() > 0) {

    int servo1pos = Serial.parseInt(); // look for the next valid integer in the incoming serial stream
    int servo2pos = Serial.parseInt(); // do it again

    // look for the newline. That's the end of your sentence
    if (Serial.read() == '\n') {
      // constrain the values to 0 - 180
      servo1pos = constrain(servo1pos, 0, 180);
      servo2pos = constrain(servo2pos, 0, 180);

      // write the servo position(s) to the servo(s)
      servo1.write(servo1pos);
      servo2.write(servo2pos);
    }
  }
}
