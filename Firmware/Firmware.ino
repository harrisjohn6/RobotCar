
// Include Libraries
#include "Arduino.h"
#include "BTHC05.h"
#include "DCMDriverL298.h"
#include "Wire.h"
#include "Adafruit_PWMServoDriver.h"
#include "NewPing.h"
#include "math.h"



// Pin Definitions
#define BTHC05_PIN_TXD 6
#define BTHC05_PIN_RXD 7

#define DCMOTORDRIVERL298_PIN_ENB 8
#define DCMOTORDRIVERL298_PIN_ENA 13

#define DCMOTORDRIVERL298_PIN_INT1 12
#define DCMOTORDRIVERL298_PIN_INT2 11
#define DCMOTORDRIVERL298_PIN_INT3 10
#define DCMOTORDRIVERL298_PIN_INT4 9

#define HCSR04_1_PIN_ECHO 5
#define HCSR04_1_PIN_TRIG 4
#define HCSR04_2_PIN_ECHO 3
#define HCSR04_2_PIN_TRIG 2

// Global variables and defines

// object initialization
BTHC05 bt(BTHC05_PIN_RXD, BTHC05_PIN_TXD);
DCMDriverL298 motorDriver(DCMOTORDRIVERL298_PIN_ENA, DCMOTORDRIVERL298_PIN_INT1, DCMOTORDRIVERL298_PIN_INT2, DCMOTORDRIVERL298_PIN_ENB, DCMOTORDRIVERL298_PIN_INT3, DCMOTORDRIVERL298_PIN_INT4);
NewPing frontDistanceSensor(HCSR04_1_PIN_TRIG, HCSR04_1_PIN_ECHO);
NewPing rearDistanceSensor(HCSR04_2_PIN_TRIG, HCSR04_2_PIN_ECHO);
Adafruit_PWMServoDriver PwmDriver = Adafruit_PWMServoDriver();

// define vars for testing menu
const int timeout = 10000;  //define timeout of 10 sec
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
const char btCharMode = 'M';
const char btStop4 = 'N';

const int servoRotMIN = 125;
const int servoRotMAX = 600;
const int driveForward = 1;
const int driveBackward = 0;
int frontDistance;
int rearDistance;
boolean driveMode;

char btCommand;

long time0;
// Speed range between 0 and 255
int spdMin = 100;
int spdMax = 255;
int spd;
int spdIncValue = 3;

byte svRotate = 0;
byte svGrapper = 2;
byte svLift = 1;
byte svExtend = 3;

void setup() {
  Serial.begin(9600);
  bt.begin(9600);
  PwmDriver.begin();
  PwmDriver.setPWMFreq(60);
  spd = 255;
}

void loop() {
  if (bt.available()) {
    btCommand = bt.read();   
  }
  
  if (Serial.available() > 0) {
    int ang = Serial.readStringUntil('\n').toInt();
    PwmDriver.setPWM(svRotate, 0, angleToPulse(ang));
    // drive(btCommand);
  }

  Serial.println(btCommand);

 }

int angleToPulse(int ang) {
  int pulse = map(ang, 1, 179, servoRotMIN, servoRotMAX);  // map angle of 0 to 180 to Servo min and Servo max
  Serial.print("Angle: ");
  Serial.print(ang);
  Serial.print(" pulse: ");
  Serial.println(pulse);
  return pulse;
}

void drive(char direction) {
  Serial.println("Entered Drive Mode");
  switch (direction) {
    case btIntUp:
      motorDriver.setMotorA(spd, driveForward);
      motorDriver.setMotorB(spd, driveForward);
      break;
    case btIntDown:
      motorDriver.setMotorA(spd, driveBackward);
      motorDriver.setMotorB(spd, driveBackward);
      break;
    case btIntLeft:
      motorDriver.setMotorA(spd, driveBackward);
      motorDriver.setMotorB(spd, driveForward);
      break;
    case btIntRight:
      motorDriver.setMotorA(spd, driveForward);
      motorDriver.setMotorB(spd, driveBackward);
      break;
    case btCharMode:
      driveMode = btCharMode;
      Serial.println("Left Drive Mode: ");
      break;
      case btStop2:
      driveMode = btCharMode;
      motorDriver.stopMotorA();
      motorDriver.stopMotorB();
      break;

    default:
      break;
  }
}

void robotArm(char mv) {
  int angInc = 1;
  int ang = 90;

  switch (mv) {
    case btIntUp /* constant-expression */:
      ang = ang + angInc;
      PwmDriver.setPWM(svLift, 0, angleToPulse(ang));
      break;

    default:
      break;
  }
}