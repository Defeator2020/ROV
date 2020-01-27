#include <Wire.h>
#include <Servo.h>
#include <nunchuck_funcs.h>

// Initialize variables for storing the Nunchuck's state
float joyy;
int valueL = 0;

Servo motor;

void setup() {
  // Initialize Nunchuck
  nunchuck_setpowerpins();
  nunchuck_init(); // send the initilization handshake
  
  // Assign the servos to their pins
  motor.attach(8);

  motor.write(140);
  delay(4000);
  
  motor.write(40);
  delay(4000);
}

void loop() {
  // Poll and assign Nunchuck states to associated variables
  nunchuck_get_data();
  joyy = nunchuck_joyy();     //  29 - 229

  joyy = (joyy / 1.1) - 27;
  
  // Lower joystick values into correct range
  joyy -= 90;
  
  // Constrain joystick range to -90 - 90 (BE WEARY OF THIS -> MIGHT WRAP AROUND. NEED TO FIX THAT)
  joyy = constrain(joyy, -90, 90);

  // Convert the positions back into the servo-friendly range of (0 - 180)
  joyy += 40;

  motor.write(joyy);
}
