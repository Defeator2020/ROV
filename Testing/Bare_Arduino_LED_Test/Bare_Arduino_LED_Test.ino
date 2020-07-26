#include <Wire.h>

int ledPin = 3;    // LED connected to digital pin 3
float ledValue = 150; // LED is inverted from usual
int ledMax = 200; // 0 - 255 (WRITE COMMAND TO USE SERIAL TO FIND ABSOLUTE MAX / MIN VALUES FOR THIS)
int ledMin = 0; // 0 - 255

void setup() {
  
  pinMode(ledPin, OUTPUT);
}

void loop() {

  ledValue = constrain(ledValue,ledMin,ledMax);

  analogWrite(ledPin, ledValue);
  delay(10);
}
