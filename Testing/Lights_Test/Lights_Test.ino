#include <Wire.h>
#include <nunchuck_funcs.h>

// Initialize variables for storing the Nunchuck's state
float joyy;

int ledPin = 9;    // LED connected to digital pin 9
float ledValue = 180; // LED is inverted from usual
int ledMax = 255; // 0 - 255 (WRITE COMMAND TO USE SERIAL TO FIND ABSOLUTE MAX / MIN VALUES FOR THIS)
int ledMin = 0; // 0 - 255

void setup() {
  // Initialize Nunchuck
  nunchuck_setpowerpins();
  nunchuck_init(); // send the initilization handshake
}

void loop() {
  getJoystickValues();

  joyy = constrain(joyy,-1,1); // APPLY BUFFERING AND WHATNOT - CALCULATIONS DONE HERE OR ONBOARD?
  ledValue += joyy * -1; // -1 is needed because the PWM controller is inverted (255(ish) = min, 0(ish) = max)
  ledValue = constrain(ledValue,ledMin,ledMax);
  
  analogWrite(ledPin, ledValue);
  delay(10);

}

void getJoystickValues() {
  // Poll and assign Nunchuck states to associated variables
  nunchuck_get_data();
  joyy = nunchuck_joyy();     //  29 - 229

  // Adjust joystick range to -90 - 90
  joyy = (joyy / 1.1) - 27;
  
  // Lower joystick values into correct range
  joyy -= 90;
  
  // Create joystick deadzone
  if (joyy > -10 && joyy < 10) {
    joyy = 0;
  }
  
  // Constrain joystick range to -90 - 90
  joyy = constrain(joyy, -90, 90);
}
