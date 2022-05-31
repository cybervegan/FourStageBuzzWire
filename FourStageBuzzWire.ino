/*
Four Stage Buzz Wire game

(C) Telford Makerspace 2022, written by Andy DJ

The wire has four segments - get as far as you can to win prizes:
  Segment 1: No prize
          2: A sticker
          3: Keyfob, badge, pen, gummies, etc.
          4: Top prize - electronics kit, 3D pen, etc.

Rules.
1. Push START button to begin.
2. You have 60 seconds to move the wand to the end of the buzz
wire, without touching it. If your time runs out, you get ONE buzz and no prize.
3. If you get to the end YOU MUST touch the wire to register your win.
4. If you touch the wire, the buzzer will sound and show the
segment of the wire that you touched it on, and a prize is 
allocated depending on how far you got.

Electronics:

Analog pins 0-3 are connected to the four segments, with a 10k pull-down resistor
and the electrode wand is connected to 5V. When in play, the game senses contact
with each of the segments by scanning the analog pins for voltage.

A pezio sounder is attached to pin 8 and ground.

A push button is connected between pin 12 and 13: pin 13 is set
HIGH and the game waits for a push on the button, taking pin 12
HIGH (it's held low by a 10k pull-down resistor).

A TM1637 4-digit 7-segment tube display is connected on pins 2 (CLK) and 
3 (DIO), +5V, and ground, and is used to display the timeout milliseconds 
remaining, and the segment where contact was made.

Wiring:

Arduino
=======

5V  RED - Display VCC
3V3  
5V  WHT (4 conductor flex) - Wand electrode
GND BRN - Display GND
GND BLK - Buzz wire board ground to Black wire

A0  GRN - Buzz wire board segment 1 to Green wire
A1  PNK - Buzz wire board segment 2 to Pink wire
A2  BLU - Buzz wire board segment 3 to Blue wire
A3  RED - Buzz wire board segment 4 to Red wire

0   
1   
2   YEL - Display CLK
3   ORG - Display DIO
4   
5   
6   
7    
8   RED - Buzzer
9   
10  
11  
12  GRY - Buzz wire board segment 3 to Grey wire
13  WHT - Buzz wire board segment 3 to White wire
GND BLK - Buzzer

Buzz wire board
===============

Left-hand side connects to buzz wire segments and start switch
Right-hand side connects to Arduino

1 RED - Buzz wire segment 4
2 BLU - Buzz wire segment 3
3 PNK - Buzz wire segment 2
4 GRN - Buzz wire segment 1
5 BLK - Ground, to pull-down resistors
6 GRY - Start button
7 WHT - Start button
8

*/

#include <TM1637.h>
#define NOTE  762

// Instantiation and pins configurations
// Pin 3 - > DIO
// Pin 2 - > CLK
TM1637 tm(2, 3);

// constants won't change. They're used here to set pin numbers:
const int buttonPin = A0;     // the number of the pushbutton pin
const int btnPower =  13;      // the number of the LED pin

const byte wirePins[] = { A0,A1,A2,A3 };
const byte btnPin = 12;
const int wirePinLength = sizeof(wirePins);
const unsigned long DEADLINE = 60000;
 
const int IDLE = 1;
const int START = 2;
const int PLAY = 3;
const int BUZZ = 4;

// variables will change:
int state = IDLE;
int contact = 0;         // variable for reading the pushbutton status
unsigned long start;

void setup() {
//  Serial.begin(9600);
  Serial.println("Init...");
  // Switchable power for the start - because I ran out of 5V pins!
  pinMode(btnPower, OUTPUT);
  pinMode(btnPin,INPUT);
  // Supply power to the start button
  digitalWrite(btnPower,HIGH);  
  // Initialise wire pins as inputs
  for (int pin=0; pin <wirePinLength; pin++) {
    pinMode(wirePins[pin], INPUT); 
  }
  tm.begin();
  tm.display("RDY");
  state = IDLE;
}

int check_cont() {
  // Scan wire pins
  int pin;
  int stateRead;
  for (pin=0; pin < wirePinLength; pin++) {
    stateRead=analogRead(wirePins[pin]); 
    if (stateRead>1000) {
      return(pin+1);
    }    
  }
  return(0);
}

void buzz() {

  Serial.println("buzz");
  for (int n=0;n<100;n++) {
    tone(8,NOTE,4);
    noTone(8);
    delay(3);
  }
  delay(250);
  
}

void loop() {
  unsigned long elapsed;
  int n;
  //Serial.print("state:"); Serial.println(state);
  switch (state){
    case IDLE:
//      Serial.println(digitalRead(btnPin));
      while (digitalRead(btnPin) == LOW) {
        // Wait for button to be released
        while (digitalRead(btnPin) == HIGH) {
          delay(1);
        }
      }
      state = START;
      break;
    case START:
      tm.display("0000");
      start = millis();
      contact = 0;
      state = PLAY;
      break;
    case PLAY:
      elapsed = millis() - start;
      contact=check_cont();
      tm.display(DEADLINE - elapsed,false,true);
      if (elapsed > DEADLINE) {
        Serial.println("TIMEOUT");
        state = BUZZ;
      } else if (contact != 0 ){
        Serial.println("CONTACT");
        state = BUZZ;
      }
      break;
    case BUZZ:
      tm.clearScreen();
      tm.display(contact);
      buzz();
      for (n=1; n < contact; n++) {
        buzz();
      }
      tm.display(contact);
      state = IDLE;
      break;
  }
}
