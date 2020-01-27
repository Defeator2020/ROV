#include <Wire.h>
#include <Servo.h>
#include <nunchuck_funcs.h>

// Initialize the values for storing the Nunchuck's state
byte joyx, joyy, zbut, cbut, accx, accy, accz;

// create variables to store the servo positions
int servo1pos = 90;
int servo2pos = 90;
int servo3pos = 90;

void setup() {
  // Initialize Nunchuck
  nunchuck_setpowerpins();
  nunchuck_init(); // send the initilization handshake

  // Open serial connection
  Serial.begin(9600);
}

void loop() {
  // Poll and assign Nunchuck states to associated variables
  nunchuck_get_data();
  joyx = nunchuck_joyx();     //  15 - 221
  joyy = nunchuck_joyy();     //  29 - 229

  joyx = (joyx / 1.1) - 23;
  joyy = (joyy / 1.1) - 27;
  
  // joyx -= 22;
  // joyy -= 26;
  
  joyx = constrain(joyx, 0, 180);
  servo1pos = joyx;
  joyy = constrain(joyy, 0, 180);
  servo2pos = joyy;

  Serial.print(servo1pos);
  Serial.print(",");
  Serial.print(servo2pos);
  Serial.print(",");
  Serial.print(servo3pos);
  Serial.print("\n");
}
