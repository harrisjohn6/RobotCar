
// Include Libraries
#include "Arduino.h"
#include "BTHC05.h"
#include "DCMDriverL298.h"
#include "NewPing.h"
#include "math.h"


// Pin Definitions
#define BTHC05_PIN_TXD  10
#define BTHC05_PIN_RXD  11

#define DCMOTORDRIVERL298_PIN_ENB   6
#define DCMOTORDRIVERL298_PIN_ENA   5

#define DCMOTORDRIVERL298_PIN_INT1  2
#define DCMOTORDRIVERL298_PIN_INT2  3
#define DCMOTORDRIVERL298_PIN_INT3  4
#define DCMOTORDRIVERL298_PIN_INT4  7

#define HCSR04_1_PIN_ECHO 9
#define HCSR04_1_PIN_TRIG 8
#define HCSR04_2_PIN_ECHO 12
#define HCSR04_2_PIN_TRIG 13




// Global variables and defines

// object initialization
BTHC05 btSensor(BTHC05_PIN_RXD, BTHC05_PIN_TXD);
DCMDriverL298 motorDriver(DCMOTORDRIVERL298_PIN_ENA, DCMOTORDRIVERL298_PIN_INT1, DCMOTORDRIVERL298_PIN_INT2, DCMOTORDRIVERL298_PIN_ENB, DCMOTORDRIVERL298_PIN_INT3, DCMOTORDRIVERL298_PIN_INT4);
NewPing frontDistanceSensor(HCSR04_1_PIN_TRIG, HCSR04_1_PIN_ECHO);
NewPing rearDistanceSensor(HCSR04_2_PIN_TRIG, HCSR04_2_PIN_ECHO);


// define vars for testing menu
const int timeout = 10000;       //define timeout of 10 sec
// constant bluetooth signals (gamepad)
const char btIntLeft = '1';
const char btIntRight = '2';
const char btIntUp = '3';
const char btIntDown = '4';
const char btIntTriangle = '5';
const char btIntSquare = '6';
const char btIntX = '7';
const char btIntO = '8';
const char btIntF1 = '9';
const char btCharF2 = 'a';
const char btStop1 = '0';
const char btStop2 = 'o';
const char btStop3 = 'M';
const char btStop4 = 'N';

const int driveForward = 1;
const int driveBackward = 0;

char btCommand;


long time0;
// Speed range between 0 and 255
int spdMin = 100;
int spdMax = 255;
int spd;
int spdIncValue = 5;

void setup()
{
  Serial.begin(9600);
  btSensor.begin(9600);
}

// Main logic of your circuit. It defines the interaction between the components you selected. After setup, it runs over and over again, in an eternal loop.
void loop()
{
  long frontFreeDistance = frontDistanceSensor.ping_cm();
  long rearFreeDistance = rearDistanceSensor.ping_cm();

  if (btSensor.available()) {
    char btCommand = btSensor.read();
    if (btCommand) {
      btAction(btCommand, frontFreeDistance, rearFreeDistance);
    }
  }
}

void btAction(char sentCommand, long frontFreeDistance, long rearFreeDistance) {

  bool stopSignal = btStopReceived(sentCommand);
  spd = getSpeed();

  if (stopSignal)
  {
    motorDriver.stopMotors();
  } else {
    if (sentCommand == btIntUp && frontFreeDistance >= 50) {
      motorDriver.setMotorA(spd, driveForward);
      motorDriver.setMotorB(spd, driveForward);
    }
    if (sentCommand == btIntDown && rearFreeDistance >= 50) {
      motorDriver.setMotorA(spd, driveBackward);
      motorDriver.setMotorB(spd, driveBackward);
    }
    if (sentCommand == btIntLeft) {
      motorDriver.setMotorA(spd, driveBackward);
      motorDriver.setMotorB(spd, driveForward);
    }
    if (sentCommand == btIntRight) {
      motorDriver.setMotorA(spd, driveForward);
      motorDriver.setMotorB(spd, driveBackward);
    } else if (sentCommand == btIntTriangle || sentCommand == btIntO) {
        setSpeed(sentCommand);
    }
  }
}

bool btStopReceived(char SentCommand) {
  if (SentCommand == btStop1) {
    return true;
  } else if (SentCommand == btStop2) {
    return true;
  } else if (SentCommand == btStop3) {
    return true;
  } else if (SentCommand == btStop4) {
    return true;
  } else {
    return false;
  }
}

void setSpeed(char spdChangeDirection) {

switch (spdChangeDirection)
{
case btIntTriangle:
  spd += spdIncValue; 
  break;
case btIntO:
spd -= spdIncValue;
break;
default:
spd = spd;
  break;
}
}

int getSpeed() {
  if (spd > spdMax) {
    return spdMax;
  } else if (spd < spdMin     ) {
    return spdMin; }
    else {
      return spd;
    }
}