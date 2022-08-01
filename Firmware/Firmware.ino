
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
BTHC05 bluetooth(BTHC05_PIN_RXD, BTHC05_PIN_TXD);
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
int frontDistance;
int rearDistance;

char btCommand;

long time0;
// Speed range between 0 and 255
int spdMin = 100;
int spdMax = 255;
int spd;
int spdIncValue = 3;

void setup()
{
  Serial.begin(9600);
  bluetooth.begin(9600);
  spd = 100;
}

void loop()
{
  getSpeed();
  getFrontDistance();
  getRearDistance();

  if (Serial.available() > 0)
  {
    btCommand = Serial.read();
    setDistance();

    switch (btCommand)
    {
    case btIntTriangle:
      setSpeed(btIntTriangle);
      break;
    case btIntX:
      setSpeed(btIntX);
      break;
    case btIntUp:
      setDistance();
      if (frontDistance >= 50)
      {
      motorDriver.setMotorA(spd, driveForward);
      motorDriver.setMotorB(spd, driveForward);
      setDistance();
      } else {
        motorDriver.stopMotors();
      }
      break;
    case btIntDown:
      setDistance();
      if (rearDistance >= 50)
      {
      motorDriver.setMotorA(spd, driveBackward);
      motorDriver.setMotorB(spd, driveBackward);
      setDistance();
      } else {
        motorDriver.stopMotors();
      }
      break;
    case btIntLeft:
      motorDriver.setMotorA(spd, driveBackward);
      motorDriver.setMotorB(spd, driveForward);
      break;
    case btIntRight:
      motorDriver.setMotorA(spd, driveForward);
      motorDriver.setMotorB(spd, driveBackward);
      break;
    default:
      break;
    }
  }

}

void setSpeed(char adjDirection)
{
  switch (adjDirection)
  {
  case btIntTriangle:
    spd =+ spdIncValue;
    break;
  case btIntX:
    spd =- spdIncValue;
    break;
  default:
    spd = spd;
    break;
  }
  if (spd < spdMin) {
    spd = spdMin;
  }
  if (spd > spdMax) {
    spd = spdMax;
  }
}

int getSpeed()
{
  return spd;
  Serial.print("The returned speed is: ");
  Serial.println(spd);
}

void setDistance()
{
  frontDistanceSensor.ping();
  frontDistance = frontDistanceSensor.ping_in();
  rearDistanceSensor.ping();
  rearDistance = rearDistanceSensor.ping_in();
}

long getFrontDistance()
{
  return frontDistance;
  Serial.print("Front distance is: ");
  Serial.println(frontDistance);
}

long getRearDistance()
{
  return rearDistance;
    Serial.print("Rear distance is: ");
  Serial.println(rearDistance);
}