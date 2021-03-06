#include <Servo.h>
#include <Wire.h>
#include <nunchuck_funcs.h>

// Initialize variables for storing the Nunchuck's state
float joyx, joyy, zbut, cbut;

Servo thruster;

// Create variables to store the various motor and servo states and turning / rate parameters
int pos1 = 0; // DO THESE VALUES NEED TO BE + AND - (remove the "+= 90" bits), OR ALL + (leave those operations)?????
int pos2 = 0;
int pivotSpeed; // Pivot Speed (-90 - 90)
float pivotScale; // Balance scale between drive (turn) and pivot (0 - 1)
float pivotYlimit = 23; // The threshold where pivoting begins, in units of the distance on the y-axis from the x-axis. Higher = more range maps to pivoting (0 - 90)

float ratePercent = 1; // Adjusts speed of motors based on rotary selector position (0 - 1)
int thrusterMinValue = 0; // Sets minimum throttle level for ESC - REMEMBER TO SET PRIOR TO TESTING (0 - 180)
int thrusterMaxValue = 180; // Sets maximum throttle level for ESC - REMEMBER TO SET PRIOR TO TESTING (0 - 180)

int peripheralControl = 0; // Tells the ROV's Arduino if the values are for the thrusters or for an accessory (0=thrusters, 1=camera, 2=lights)

void setup() {
  // Initialize Nunchuck
  nunchuck_setpowerpins();
  nunchuck_init(); // send the initilization handshake

  thruster.attach(8,700,2000);
}

void loop() {
    ratePercent = .5;
    getJoystickValues();
    calculateThrusters();
    adjustMotorRange();

  
  // Send those values over serial
  thruster.write(pos1);
}

void getJoystickValues() {
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
  
  // Constrain joystick range to -90 - 90
  joyx = constrain(joyx, -90, 90);
  joyy = constrain(joyy, -90, 90);
}

void adjustMotorRange() {
  // Calculate the new maximum and minimum (range extents) for the values to be sent to the ESC
  int rangeAdjust = thrusterMaxValue - thrusterMinValue;
  rangeAdjust = (rangeAdjust / 180);
  pos1 *= rangeAdjust; // Adjust the thruster values by the calculated adjustment factor
  pos2 *= rangeAdjust;
  
  pos1 += 90; // Move the adjusted values up into the (0 - 180) range
  pos2 += 90;
  
}

void calculateThrusters() {
  // Calculate DRIVE TURN output, based on X input
  if (joyy >= 0) { // We are going FORWARD
    if (joyx >= 0) {
      pos1 = 90;
      pos2 = 90 - joyx;
    } else {
      pos1 = 90 + joyx;
      pos2 = 90;
    }
  } else { // We are going BACKWARD
    if (joyx >= 0) {
      pos1 = 90 - joyx;
      pos2 = 90;
    } else {
      pos1 = 90;
      pos2 = 90 + joyx;
    }
  }

  // Scale DRIVE output, based on Y input ('throttle')
  pos1 = pos1 * (joyy/90);
  pos2 = pos2 * (joyy/90);

  // Calculate PIVOT amount (strength based on X input, blending based on Y input)
  pivotSpeed = joyx;
  if (abs(joyy) > pivotYlimit) {
    pivotScale = 0;
  } else {
    pivotScale = 1 - abs(joyy)/pivotYlimit;
  }

  // Finalize mix of DRIVE (TURN) and PIVOT
  pos1 = (1 - pivotScale) * pos1 + pivotScale * (pivotSpeed);
  pos2 = (1 - pivotScale) * pos2 + pivotScale * (-pivotSpeed);

  // Adjust values based on current rate setting
  pos1 = pos1 * ratePercent;
  pos2 = pos2 * ratePercent;
}
