#include <Wire.h>

int ledPin = 4;    // LED connected to digital pin 4
float ledValue = 100; // LED is inverted from usual
int ledMax = 255; // 0 - 255 (WRITE COMMAND TO USE SERIAL TO FIND ABSOLUTE MAX / MIN VALUES FOR THIS)
int ledMin = 0; // 0 - 255

void setup() {
  pinMode(3, OUTPUT);
  Serial.begin(9600);
  digitalWrite(3, LOW);

}

void loop() {
  
  ledValue = constrain(ledValue,ledMin,ledMax);

  analogWrite(ledPin, ledValue);
  delay(10);
}
