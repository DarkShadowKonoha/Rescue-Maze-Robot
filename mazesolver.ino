#include <NewPing.h>

// 0 - left
// 1 - right
// 2 - front

//motors
#define LMF 5
#define LMR 6
#define RMF 10
#define RMR 11

//#define DEBUG

#define STOP

#define turnduration 450

#define hazardbackdistance 350

#define alignduration 1000

#define frontwall 6

//change with caution
#define leftturntime 110

#define sidewall 8.5

#define MAX_DISTANCE 300 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.


#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif



String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete

unsigned long previousMillis = 0;        // will store last time LED was updated

float distance[3];
const int trig[3] = {9, 8, 7};
const int echo[3] = {9, 8, 7};

NewPing sonar0(trig[0], echo[0], MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing sonar1(trig[1], echo[1], MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing sonar2(trig[2], echo[2], MAX_DISTANCE); // NewPing setup of pins and maximum distance.


void setup() {
  Serial.begin(115200);

  inputString.reserve(200);

  pinMode(A0, INPUT);

  pinMode(LMF, OUTPUT);
  pinMode(RMF, OUTPUT);
  pinMode(LMR, OUTPUT);
  pinMode(RMR, OUTPUT);

  pinMode(2, INPUT_PULLUP);
#ifdef STOP
  while (digitalRead(2)) {
    Serial.println(digitalRead(A0), DEC);
    delay(100);
  }
#endif
  delay(2000);
  moveAligned(80, 80);
}

void loop() {

  moveAligned(80, 80);

  if (digitalRead(A0)) {
    moveMotor(0, 0);
    delay(100);
    moveMotor(-61, -60);
    delay(hazardbackdistance);
    if (getDistance(0) > getDistance(1)) {
      moveMotor(80, -80);
      delay(turnduration);
      moveMotor(0, 0);
      delay(200);
      moveMotor(-80, -80);
      delay(alignduration);
      moveAligned(80, 80);
    } else {
      moveMotor(-80, 80);
      delay(turnduration);
      moveMotor(0, 0);
      delay(200);
      moveMotor(-80, -80);
      delay(alignduration);
      moveAligned(80, 80);
    }
  }

  if (getDistance(2) < frontwall ) {
    moveMotor(0, 0);
    moveMotor(-51, -50);
    delay(100);

    if (getDistance(0) > 25 && getDistance(1) > 25) {
      moveMotor(-80, 80);
      delay(turnduration);
      moveMotor(0, 0);
      delay(200);
      moveAligned(80, 80);
    } else if (getDistance(0) > getDistance(1)) {
      moveMotor(80, -80);
      delay(turnduration);
      moveMotor(0, 0);
      delay(200);
      moveMotor(-80, -80);
      delay(alignduration);
      moveAligned(80, 80);
    } else {
      moveMotor(-80, 80);
      delay(turnduration);
      moveMotor(0, 0);
      delay(200);
      moveMotor(-80, -80);
      delay(alignduration);
      moveAligned(80, 80);
    }
  }

  if (getDistance(1) < 15 && getDistance(2) > 50) {
    Serial.println("turn detected");
    for (int i = 0; i < leftturntime; i++) {
      Serial.println(i, DEC);
      if (getDistance(2) < 10) {
        break;
      }
      moveAligned(80, 80);
    }

    if (getDistance(1) > 25) {
      Serial.println("turn left");

      moveMotor(-80, 80);
      delay(turnduration);
      moveMotor(0, 0);
      delay(200);
      moveMotor(-80, -80);
      delay(800);
      moveAligned(80, 80);
    }
  }
}

void moveMotor(int m1Speed, int m2Speed) {
  if (m1Speed > 0 ) {
    analogWrite(LMF, m1Speed);
    analogWrite(LMR, 0);
  } else {
    analogWrite(LMF, 0);
    analogWrite(LMR, -m1Speed);
  }
  if (m2Speed > 0) {
    analogWrite(RMF, m2Speed);
    analogWrite(RMR, 0);
  } else {
    analogWrite(RMF, 0);
    analogWrite(RMR, -m2Speed);
  }
}

float getDistance(int i) {

  float dist = getdist(i);

  int z = 0;

  while (dist < 1 && z < 5) {
    dist = getdist(i);
    z++;
  }

  if (z >= 5) {
    dist = 100;
  }
#ifdef DEBUG
  if (distance[i] > 0) {
    Serial.print(distance[i], DEC);
    Serial.print(" ");
  }
#endif

  return dist;
}


float getdist(int i) {
  int dist;
  switch (i) {
    case 0: dist = sonar2.ping_cm();
      break;
    case 1: dist = sonar1.ping_cm();
      break;
    case 2: dist = sonar0.ping_cm();
      break;
    default:
      break;
  }
  return dist;
}

void moveAligned(int LS, int RS) {
  int L = getDistance(1);
  int R = getDistance(0);

  int factor;

  if (L < 20 && R < 20) {
    factor = (L - R) * 1.5;
    moveMotor(LS - factor, RS + factor);
  } else if (L < 20 && R > 20) {
    factor = (L - sidewall) * 1.5;
    moveMotor(LS - factor, RS + factor);
  } else if (L > 20 && R < 20) {
    factor = (R - sidewall) * 1.5;
    moveMotor(LS + factor, RS - factor);
  } else {
    moveMotor(LS, RS);
  }
}



void moveAlignedUntil(int LS, int RS, int T) {
  previousMillis = millis();
  while (1) {
    moveAligned(LS, RS);
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= T) {
      // save the last time you blinked the LED
      previousMillis = currentMillis;
      break;
    }
  }
}

