#include <Wire.h>
#include <nunchuck_funcs.h>

// Initialize variables for storing the Nunchuck's state
float joyy;

int ledPin = 4;    // LED connected to digital pin 9
float ledValue = 100; // LED is inverted from usual
int ledMax = 255; // 0 - 255 (WRITE COMMAND TO USE SERIAL TO FIND ABSOLUTE MAX / MIN VALUES FOR THIS)
int ledMin = 0; // 0 - 255

void setup() {
  // Initialize Nunchuck
  // nunchuck_setpowerpins();
  // nunchuck_init(); // send the initilization handshake

  pinMode(3, OUTPUT);
  Serial.begin(9600);
  digitalWrite(3, LOW);

}

void loop() {
  while (Serial.available() > 0) {
  int joyy = Serial.parseInt();

  if (Serial.read() == '\n') {
    joyy = constrain(joyy,-1,1); // APPLY BUFFERING AND WHATNOT - CALCULATIONS DONE HERE OR ONBOARD?
    ledValue += joyy * -1; // -1 is needed because the PWM controller is inverted (255(ish) = min, 0(ish) = max)
    ledValue = constrain(ledValue,ledMin,ledMax);

    analogWrite(ledPin, ledValue);
    delay(10);
    }
  } 
}
