#include <IRremote.h>
#include "TM1637.h"

#define IR_RECEIVE_PIN 7
#define CLK 4
#define DIO 5
#define encoderPinA  2
#define encoderPinB  3
#define LED 13
#define BUTTON 6


TM1637 tm1637(CLK,DIO);

volatile int pos = 0;
int currentPos = 0;
unsigned long keycode, t1, dt;

const int stepPin = 11;
const int dirPin = 10;
const int enPin = 8;

int stato = HIGH;

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);

  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, HIGH);
  digitalWrite(dirPin, LOW);
  digitalWrite(LED, LOW);

  // encoder pin on interrupt 0 (pin 2)
  attachInterrupt(0, doEncoderA, CHANGE);
  // encoder pin on interrupt 1 (pin 3)
  attachInterrupt(1, doEncoderB, CHANGE);
  
  //Serial.begin(9600); 
  IrReceiver.begin(IR_RECEIVE_PIN);
  //Serial.println("ok");
  
  tm1637.init();
  //BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
  tm1637.set(BRIGHT_TYPICAL);
  //tm1637.point(POINT_ON);
  shownumber(pos); 
  t1 = 0; 
}


void loop() {
  dt = millis() - t1;
  if (dt >= 5000 && stato == LOW){      
      digitalWrite(enPin, HIGH);
      digitalWrite(LED, LOW);
      stato = !stato;
  }
  
  if (IrReceiver.decode()) {
    keycode = IrReceiver.decodedIRData.command;
    //Serial.println(keycode);
    if ((IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)) {
         IrReceiver.resume();
         return;
    }
    IrReceiver.resume();
  }


   if (digitalRead(BUTTON) == LOW) {
      delay(300);
      stato = !stato;
      digitalWrite(enPin, stato);
      digitalWrite(LED, !stato);
      t1 = millis();
      currentPos = pos; 
   }


   if (keycode == 31) {
      delay(300);
      stato = !stato;
      digitalWrite(enPin, stato);
      digitalWrite(LED, !stato);
      keycode = 0;
      t1 = millis();
      currentPos = pos;
      }

   
   if (keycode == 29 && stato == LOW) { 
    pos --;
    if (pos < 0) pos = 0;
    keycode = 0;
      
  }
  
if (keycode == 30 && stato == LOW) { 
    pos ++;
    keycode = 0;
       
  }

if (currentPos != pos && stato == LOW) {
    t1 = millis(); 
    shownumber(pos);
    if (currentPos < pos) {
      
      digitalWrite(dirPin,HIGH);
      digitalWrite(stepPin,HIGH);
      delayMicroseconds(1000);
      digitalWrite(stepPin,LOW);
      delayMicroseconds(1000);
      
      currentPos = pos;
    }
    else if (currentPos > pos && stato == LOW) {
      
      digitalWrite(dirPin,LOW);
      digitalWrite(stepPin,HIGH);
      delayMicroseconds(1000);
      digitalWrite(stepPin,LOW);
      delayMicroseconds(1000);
      
      currentPos = pos;
      }
  }
  
}

void shownumber(int n){

  int d1 = n % 10;
  int d2 = (n/10) % 10;
  int d3 = (n/100) % 10;
  tm1637.display(0,d3);
  tm1637.display(1,d2);
  tm1637.display(2,d1);
}

void doEncoderA() {
  if (stato == LOW) {
  // look for a low-to-high on channel A
  if (digitalRead(encoderPinA) == HIGH) {
    // check channel B to see which way encoder is turning
    if (digitalRead(encoderPinB) == LOW) {
      pos --;         // CW
      if (pos < 0) pos = 0;
    } else {
      pos ++;         // CCW
    }
  } else {// look for a high-to-low on channel A
    if (digitalRead(encoderPinB) == HIGH) {// check channel B to see which way encoder is turning
      pos --;          // CW
      if (pos < 0) pos = 0;
    } else {
      pos ++;          // CCW
    }
  }  
}
}

void doEncoderB() {
  if (stato == LOW) {
  // look for a low-to-high on channel B
  if (digitalRead(encoderPinB) == HIGH) {
    // check channel A to see which way encoder is turning
    if (digitalRead(encoderPinA) == HIGH) {
      pos --;         // CW
      if (pos < 0) pos = 0;
    } else {
      pos ++;         // CCW
    }
  } else { // Look for a high-to-low on channel B
    // check channel B to see which way encoder is turning
    if (digitalRead(encoderPinA) == LOW) {
      pos --;          // CW
      if (pos < 0) pos = 0;
    } else {
      pos ++;          // CCW
    }
  }
}
}
