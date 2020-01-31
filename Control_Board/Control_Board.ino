#include <Wire.h>
#include <nunchuck_funcs.h>

// Initialize variables for storing the Nunchuck's state
float joyx, joyy, zbut, cbut;

// Create variables to store the various motor and servo states and turning / rate parameters
int valueL = 0; // DO THESE VALUES NEED TO BE + AND - (remove the "+= 90" bits), OR ALL + (leave those operations)?????
int valueR = 0;
int valueV = 0;
int pivotSpeed; // Pivot Speed (-90 - 90)
float pivotScale; // Balance scale between drive (turn) and pivot (0 - 1)
float pivotYlimit = 23; // The threshold where pivoting begins, in units of the distance on the y-axis from the x-axis. Higher = more range maps to pivoting (0 - 90)

float ratePercent = 1; // Adjusts speed of motors based on rotary selector position (0 - 1)
int thrusterMinValue = 40; // Sets minimum throttle level for ESC - REMEMBER TO SET PRIOR TO TESTING (0 - 180)
int thrusterMaxValue = 140; // Sets maximum throttle level for ESC - REMEMBER TO SET PRIOR TO TESTING (0 - 180)

int peripheralControl = 0; // Tells the ROV's Arduino if the values are for the thrusters or for an accessory (0=thrusters, 1=camera, 2=lights)

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
  pinMode(6, OUTPUT); // DELETE ONCE JUMPERED

  // Set to only transmit on this end - REMOVE IF RECEIPT OF INFORMATION IS NECESSARY
  digitalWrite(7, HIGH);
  digitalWrite(6, HIGH); // DELETE ONCE JUMPERED
  
  // Open serial connection
  Serial.begin(9600);
}

void loop() {
  peripheralControl = 0;
  
  // Check rotary selector position
  if (digitalRead(8) == LOW) {
    ratePercent = 1;
    getJoystickValues();
    calculateThrusters();

    adjustMotorRange();
    // Convert the positions back into the motor-friendly range
  } else if (digitalRead(9) == LOW) {
    ratePercent = .5;
    getJoystickValues();
    calculateThrusters();

    adjustMotorRange();
    // Convert the positions back into the motor-friendly range
  } else if (digitalRead(10) == LOW) {
    getJoystickValues();

    valueL = joyx;
    valueR = joyy;
    
    // Convert the positions back into the servo(and ESC)-friendly range of (0 - 180)
    valueR += 90;
    
    if (cbut == 1 && zbut == 0) {
      valueV = 1;
    } else if (cbut == 0 && zbut == 1) {
      valueV = -1;
    } else {
      valueV = 0;
    }
    
    peripheralControl = 1;
  } else {
    valueL = 90; // WHAT SHOULD I DO HERE? - TEMPORARY ERROR CODE
    valueR = 90;
  }
  
  // Send those values over serial
  Serial.print(valueL);
  Serial.print(",");
  Serial.print(valueR);
  Serial.print(",");
  Serial.print(valueV);
  Serial.print(",");
  Serial.print(peripheralControl);
  Serial.print("\n");
}

void getJoystickValues() {
  // Poll and assign Nunchuck states to associated variables
  nunchuck_get_data();
  joyx = nunchuck_joyx();     //  15 - 221
  joyy = nunchuck_joyy();     //  29 - 229
  zbut = nunchuck_zbutton();     //  0 - 1
  cbut = nunchuck_cbutton();     //  0 - 1

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
  valueL *= rangeAdjust; // Adjust the thruster values by the calculated adjustment factor
  valueR *= rangeAdjust;
  valueV *= rangeAdjust;
  
  valueL += 90; // Move the adjusted values up into the (0 - 180) range
  valueR += 90;
  valueV += 90;
  
}

void calculateThrusters() {
  // Calculate DRIVE TURN output, based on X input
  if (joyy >= 0) { // We are going FORWARD
    if (joyx >= 0) {
      valueL = 90;
      valueR = 90 - joyx;
    } else {
      valueL = 90 + joyx;
      valueR = 90;
    }
  } else { // We are going BACKWARD
    if (joyx >= 0) {
      valueL = 90 - joyx;
      valueR = 90;
    } else {
      valueL = 90;
      valueR = 90 + joyx;
    }
  }

  // Scale DRIVE output, based on Y input ('throttle')
  valueL = valueL * (joyy/90);
  valueR = valueR * (joyy/90);

  // Calculate PIVOT amount (strength based on X input, blending based on Y input)
  pivotSpeed = joyx;
  if (abs(joyy) > pivotYlimit) {
    pivotScale = 0;
  } else {
    pivotScale = 1 - abs(joyy)/pivotYlimit;
  }

  // Finalize mix of DRIVE (TURN) and PIVOT
  valueL = (1 - pivotScale) * valueL + pivotScale * (pivotSpeed);
  valueR = (1 - pivotScale) * valueR + pivotScale * (-pivotSpeed);

  // Calculate 3rd motor value (valueV)
  if (cbut == 1 && zbut == 0) {
    valueV = 90; // FIGURE OUT WHAT VERTICAL RATES SHOULD BE, AND ADJUST LIMITS HERE
  } else if (cbut == 0 && zbut == 1) {
    valueV = -90;
  } else {
    valueV = 0;
  }

  // Adjust values based on current rate setting
  valueL = valueL * ratePercent;
  valueR = valueR * ratePercent;
  valueV = valueV * ratePercent;
}
