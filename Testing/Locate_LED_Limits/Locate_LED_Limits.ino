int ledPin = 9;    // LED connected to digital pin 9
float ledValue = 180; // LED is inverted from usual

int value = 100;

void setup() {
  Serial.begin(9600); // Initialize serial communication
}

void loop() {
  while (Serial.available() > 0) {
  int value = Serial.parseInt(); // Look for the next valid integer in the incoming serial stream
  
  // Look for the newline. That marks the end of the sentence
    if (Serial.read() == '\n') {
      // Constrain to reasonable limits (0 - 255)
      value = constrain(value, 0, 255);
      analogWrite(ledPin, value);
    }
  }
}
