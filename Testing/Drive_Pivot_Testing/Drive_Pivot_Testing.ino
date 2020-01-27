#include <Wire.h>
#include <Servo.h>
#include <nunchuck_funcs.h>

// Initialize the values for storing the Nunchuck's state
float joyx, joyy;

// create variables to store the servo positions and turning parameters
int servoLpos = 0;
int servoRpos = 0;
int pivotSpeed; // Pivot Speed (-90 - 90)
float pivotScale; // Balance scale between drive (turn) and pivot (0 - 1)
float pivotYlimit = 23; // The threshold where pivoting begins, in units of the distance on the y-axis from the x-axis. Higher = more range maps to pivoting (0 - 90)

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

  // Adjust joystick range to -90 - 90
  joyx = (joyx / 1.1) - 23;
  joyy = (joyy / 1.1) - 27;
  
  // Lower joystick values into correct range
  joyx -= 90;
  joyy -= 90;
  
  // Create joystick deadzone
  if (joyx > -10 && joyx < 10) {
    joyx = 0;
  }
  if (joyy > -10 && joyy < 10) {
    joyy = 0;
  }


  // Constrain joystick range to -90 - 90 (BE WEARY OF THIS -> MIGHT WRAP AROUND. NEED TO FIX THAT)
  joyx = constrain(joyx, -90, 90);
  joyy = constrain(joyy, -90, 90);

  // Calculate DRIVE TURN output, based on X input
  if (joyy >= 0) { // We are going FORWARD
    if (joyx >= 0) {
      servoLpos = 90;
      servoRpos = 90 - joyx;
    } else {
      servoLpos = 90 + joyx;
      servoRpos = 90;
    }
  } else { // We are going BACKWARD
    if (joyx >= 0) {
      servoLpos = 90 - joyx;
      servoRpos = 90;
    } else {
      servoLpos = 90;
      servoRpos = 90 + joyx;
    }
  }

  // Scale DRIVE output, based on Y input ('throttle')
  servoLpos = servoLpos * (joyy/90);
  servoRpos = servoRpos * (joyy/90);

  // Calculate PIVOT amount (strength based on X input, blending based on Y input)
  pivotSpeed = joyx;
  if (abs(joyy) > pivotYlimit) {
    pivotScale = 0;
  } else {
    pivotScale = 1 - abs(joyy)/pivotYlimit;
  }

  // Finalize mix of DRIVE (TURN) and PIVOT
  servoLpos = (1 - pivotScale) * servoLpos + pivotScale * (pivotSpeed);
  servoRpos = (1 - pivotScale) * servoRpos + pivotScale * (-pivotSpeed);

  // Convert the positions back into the servo-friendly range of 0 - 180
  servoLpos += 90;
  servoRpos += 90;

  // Send those values over serial
  Serial.print(servoLpos);
  Serial.print(",");
  Serial.print(servoRpos);
  Serial.print("\n");
}
