
//////////////
// INCLUDES
//////////////

// Necessary headers
#include <Wire.h>
#include <NXShield.h>
#include <NXTTouch.h>


/////////////////////
// GLOBAL VARIABLES
////////////////////

NXShield nxshield;
NXTTouch touch1;


//Motors
int m1Speed = 30; //Speed of the kick
int m2Speed = 30; //Speed of the left/right movement

//HallEffect sensors
int sensorPower = 11;
int sensor1 = 10;
int sensor2 = 9;


//////////////
// FUNCTIONS
//////////////

void setup()
{

  Serial.begin(115200);

  //Initializing NXShield
  nxshield.init();

  //Initializing sensors
  nxshield.bank_a.motorReset();
  nxshield.bank_b.motorReset();

  //Initializing touch sensor
  touch1.init( &nxshield, SH_BBS1 ); // initialize a touch sensor on Bank B, Sensor 1

  //HallEffect sensor
  pinMode(sensorPower, OUTPUT);
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);

  // Turn on power to the Hall Effect sensors.
  digitalWrite(sensorPower, HIGH);
}

void loop() {
  //CubeSorter starts when the touch sensor is pressed
  if (touch1.isPressed()) {
    start();
  }
}

void start() {

  //Angles
  long m2Angle = 160;
  long kickAngle = 90;

  int cubeToFind = 3; // Finding cube 3 first
  int sensorLoc = 1; // Initially sensor is in the middle

  //2D Array
  //index 0: if sensed 1 otherwise 0
  //index 1: if not sensed -1, otherwise the corresponding cube number
  //index 2: if kicked 1 otherwise 0
  int cube[3][3] = {{0, -1, 0}, {0, -1, 0}, {0, -1, 0}};

  for (int i = 0; cubeToFind > 0 || getSensorLoc(i) != 1; i++) {

    sensorLoc = getSensorLoc(i); // get the location of the sensor

    if (!cube[sensorLoc][0] || i > 6) { // if cube hasn't been sensed before
      cube[sensorLoc][0] = 1;
      cube[sensorLoc][1] = getCubeNumber();
    }


    if ( !cube[sensorLoc][2] && cube[sensorLoc][1] == cubeToFind) {
      if (cubeToFind == 1 && i > 6) {
        if (cubeNotKicked(cube) == sensorLoc) {
          cube[sensorLoc][2] = 1;
          moveForwardM1(kickAngle); // kick
          moveReverseM1(kickAngle); // return
          cubeToFind--; // find the next cube
        }
      }
      cube[sensorLoc][2] = 1;
      moveForwardM1(kickAngle); // kick
      moveReverseM1(kickAngle); // return
      cubeToFind--; // find the next cube
    }


    if (i % 4 == 0 || i % 4 == 3) {
      moveReverseM2(m2Angle); // go right
    } else {
      moveForwardM2(m2Angle); // go left
    }

  }// end for

}

//Just in case the last block is not detected properly drop the cube that hasn't been kicked
int cubeNotKicked(int cube[3][3]) {

  int lastCubeLoc;
  int i;

  for (i = 0; i < 3; i++) {
    if (!cube[i][2]) {
      lastCubeLoc = i;
    }
  }

  return lastCubeLoc;

}

int getSensorLoc(int index) {
  int loc;
  if (index % 2 == 0) {
    loc = 1;
  } else if ( index % 4 == 3 ) {
    loc = 2;
  } else {
    loc = 0;
  }
  return loc;
}

int getCubeNumber() {
  int cubeNum;

  int s1 = digitalRead(sensor1);
  int s2 = digitalRead(sensor2);

  if (s1 == 1 && s2 == 1) {
    cubeNum = 1;
    nxshield.ledSetRGB(8, 0, 0);//Red if 1
  } else if ( s1 != s2 ) {
    cubeNum = 2;
    nxshield.ledSetRGB(0, 8, 0);//Green if 2
  } else {
    cubeNum = 3;
    nxshield.ledSetRGB(0, 0, 8);//Blue if 3
  }

  return cubeNum;
}

void moveForwardM2(long angle) {
  nxshield.bank_b.motorRunDegrees(SH_Motor_2, SH_Direction_Forward, m2Speed,
                                  angle, SH_Completion_Wait_For, SH_Next_Action_Brake);
}

void moveReverseM2(long angle) {
  nxshield.bank_b.motorRunDegrees(SH_Motor_2, SH_Direction_Reverse, m2Speed,
                                  angle, SH_Completion_Wait_For, SH_Next_Action_Brake);
}

void moveForwardM1(long angle) {
  nxshield.bank_b.motorRunDegrees(SH_Motor_1, SH_Direction_Forward, m1Speed,
                                  angle, SH_Completion_Wait_For, SH_Next_Action_Brake);
}

void moveReverseM1(long angle) {
  nxshield.bank_b.motorRunDegrees(SH_Motor_1, SH_Direction_Reverse, m1Speed,
                                  angle, SH_Completion_Wait_For, SH_Next_Action_Brake);
}

