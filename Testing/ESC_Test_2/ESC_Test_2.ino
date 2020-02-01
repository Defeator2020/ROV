#include <Wire.h>
#include <nunchuck_funcs.h>
#include <Servo.h>

Servo thruster;
float joyy;

void setup() {
  thruster.attach(8,1000,2000);

  // Initialize Nunchuck
  nunchuck_setpowerpins();
  nunchuck_init(); // send the initilization handshake
}

void loop() {
  nunchuck_get_data();
  joyy = nunchuck_joyy();     //  29 - 229

  joyy = (joyy / 1.1) - 27;

  joyy = constrain(joyy, 0, 180);

  thruster.write(joyy);
}
