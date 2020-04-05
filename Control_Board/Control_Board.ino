#include <Wire.h>
#include <nunchuck_funcs.h>

// Initialize variables for storing the Nunchuck's state
float joyx, joyy, zbut, cbut;

// Create variables to store the various motor and servo states and turning / rate parameters
int pos1 = 90;
int pos2 = 90;
int pos3 = 90;
int pivotSpeed; // Pivot Speed (-90 - 90)
float pivotScale; // Balance scale between drive (turn) and pivot (0 - 1)
float pivotYlimit = 23; // The threshold where pivoting begins, in units of the distance on the y-axis from the x-axis. Higher = more range maps to pivoting (0 - 90)

float ratePercent = 1; // Adjusts speed of motors based on rotary selector position (0 - 1)
int thrusterMinValue = 0; // Sets minimum throttle level for ESC - REMEMBER TO SET PRIOR TO TESTING (0 - 180)
int thrusterMaxValue = 180; // Sets maximum throttle level for ESC - REMEMBER TO SET PRIOR TO TESTING (0 - 180)

int peripheralControl = 0; // Tells the ROV's Arduino if the values are for the thrusters or for an accessory (0=thrusters, 1=camera, 2=misc)

void setup() {
  // Initialize Nunchuck
  nunchuck_setpowerpins();
  nunchuck_init(); // send the initilization handshake

  // Set pins connected to rotary selector to inputs
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);

  // Set output pin for enabling RS485 transmit
  pinMode(7, OUTPUT);
  
  // Open serial connection
  Serial.begin(9600);
  
  // Set serial board to low (receive)
  digitalWrite(7, HIGH); // HIGH (RECEIVE-ONLY) FOR TESTING
}

void loop() {
  peripheralControl = 0;
  
  // Check rotary selector position
  if (digitalRead(8) == LOW) {
    ratePercent = 1;
    getJoystickValues(1);
    calculateThrusters();

    adjustMotorRange();
  } else if (digitalRead(9) == LOW) {
    ratePercent = .25;
    getJoystickValues(1);
    calculateThrusters();

    adjustMotorRange();
  } else if (digitalRead(10) == LOW) {
    getJoystickValues(0);

    pos1 = joyx;
    pos2 = joyy;
    
    // Convert the positions back into the servo(and ESC)-friendly range of (0 - 180)
    pos1 += 90;
    pos2 *= -1;
    pos2 += 90;

    // Send value for LED control
    if (cbut == 1 && zbut == 0) {
      pos3 = 1;
    } else if (cbut == 0 && zbut == 1) {
      pos3 = -1;
    } else {
      pos3 = 0;
    }

    peripheralControl = 1;
  } else if (digitalRead(11) == LOW) {
    // PUT CODE FOR MANIPULATOR / SAMPLING INSTRUMENTS HERE
    
  } else {
    pos1 = 90;
    pos2 = 90;
    pos3 = 90;
  }
  
	transmitData();
}

void transmitData() { // Transmit data to the vehicle over serial
  // Set high to transmit
  // digitalWrite(7, HIGH);
  
  // Send those values over serial
  Serial.print(pos1);
  Serial.print(",");
  Serial.print(pos2);
  Serial.print(",");
  Serial.print(pos3);
  Serial.print(",");
  Serial.print(peripheralControl);
  Serial.print("\n");
  
  // Return to low (receive)
  // digitalWrite(7, LOW);
}

void getJoystickValues(int deadZone) {
  // Poll and assign Nunchuck states to associated variables
  nunchuck_get_data();
  joyx = nunchuck_joyx();     //  15 - 221
  joyy = nunchuck_joyy();     //  29 - 229
  zbut = nunchuck_zbutton();     //  0 - 1
  cbut = nunchuck_cbutton();     //  0 - 1

  // Adjust joystick range to 0 - 180
  joyx = (joyx / 1.1) - 23;
  joyy = (joyy / 1.1) - 27;
  
  // Lower joystick values into correct range
  joyx -= 90;
  joyy -= 90;
  
  if (deadZone == 1) {
    // Create joystick deadzone
    if (joyx > -10 && joyx < 10) {
      joyx = 0;
    }
    if (joyy > -10 && joyy < 10) {
      joyy = 0;
    }
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
  pos3 *= rangeAdjust;
  
  pos1 += 90; // Move the adjusted values up into the (0 - 180) range
  pos2 += 90;
  pos3 += 90;
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

  // Calculate 3rd motor value (pos3)
  if (cbut == 1 && zbut == 0) {
    pos3 = 90;
  } else if (cbut == 0 && zbut == 1) {
    pos3 = -90;
  } else {
    pos3 = 0;
  }

  // Adjust values based on current rate setting
  pos1 = pos1 * ratePercent;
  pos2 = pos2 * ratePercent;
  pos3 = pos3 * ratePercent;
}
