/* Example code for TM1637 4 digit 7 segment display with Arduino. More info: www.www.makerguides.com */

// Include the library:
#include "TM1637Display.h"
#include <Encoder.h>


// Define the connections pins:
#define CLK 10
#define DIO 11

// Create display object of type TM1637Display:
TM1637Display display = TM1637Display(CLK, DIO);

// Create array that turns all segments on:
const uint8_t full[] = {0xff, 0xff, 0xff, 0xff};

// Create array that turns all segments off:
const uint8_t blank[] = {0x00, 0x00, 0x00, 0x00};

// You can set the individual segments per digit to spell words or create other symbols:
const uint8_t done[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
  SEG_D | SEG_C | SEG_G | SEG_E,                   // o
  SEG_C | SEG_E | SEG_G,                           // n
  SEG_F | SEG_E | SEG_A | SEG_G | SEG_D | SEG_B,   // e
};

const uint8_t hello[] = {
  SEG_F | SEG_E | SEG_C | SEG_G,                   // h
  SEG_F | SEG_E | SEG_A | SEG_G | SEG_D | SEG_B,   // e
  SEG_F | SEG_E | SEG_B | SEG_C,                   // ll
  SEG_G | SEG_E | SEG_C | SEG_D                    // o
};

const uint8_t bye[] = {
  SEG_F | SEG_E | SEG_D | SEG_C | SEG_G,           // b
  SEG_F | SEG_C | SEG_G | SEG_D | SEG_B,           // y
  SEG_F | SEG_E | SEG_A | SEG_G | SEG_D | SEG_B,   // e
  0x00                                             // 
};

int curr_enc;
int prev_enc;
int curr_switch;
int prev_switch;
unsigned long start_time = 0;
unsigned long duration = 0;

enum states {
  TIMING,
  SLEEPING,
  SETTING,
};


Encoder encoder(2, 3);

void beep() {
  int i;
  for (i = 0; i < 50; i++) {
    digitalWrite(8, HIGH);
    delay(1);
    digitalWrite(8, LOW);
    delay(1);
  }
}

void alarm() {
  int i;
  int j;
  for(j = 0; j < 10; j++){ 
    for (i = 0; i < 100; i++) {
      digitalWrite(8, HIGH);
      delay(2);
      digitalWrite(8, LOW);
      delay(2);
    }
    for (i = 0; i < 100; i++) {
      digitalWrite(8, LOW);
      delay(4);
    }
  }  
}

void alert() {
  int i;
  int j;
  for(j = 0; j < 3; j++){ 
    for (i = 0; i < 50; i++) {
      digitalWrite(8, HIGH);
      delay(1);
      digitalWrite(8, LOW);
      delay(1);
    }
    for (i = 0; i < 50; i++) {
      digitalWrite(8, LOW);
      delay(2);
    }
  }  
}

unsigned long millis_to_time(unsigned long in_millis){
  unsigned int minutes = in_millis/60000;
  unsigned int hours = minutes / 60;
  minutes = minutes - hours * 60;
  return hours * 100 + minutes; 
}

void draw_time(unsigned long in_millis, int flash) {
  unsigned int dec_time = millis_to_time(in_millis);
  unsigned char colon = 0b11100000;
  if(!flash){
    colon = 0;
  }
  display.showNumberDecEx(dec_time, colon, false, 4, 0);
}

void mark_start(unsigned long delay){
  start_time = millis();
  timing = 1;
  beep();
}

int get_state(){
   return state;
}

void set_state(int in_state){
  state = in_state;
}


void setup() {
  // Clear the display:
  Serial.begin(9600);
  display.clear();
  pinMode(8, OUTPUT);
  pinMode(5, INPUT);
  delay(1000);
  display.setBrightness(7);
  // All segments on:
  display.setSegments(hello);
  delay(2000);
  display.clear();
  delay(1000);
  alert();
}
void announce_time(){
  timing = 0;
  duration = 0;
  encoder.readAndReset();
  display.setSegments(done);
  alarm();
  delay(3000);
  display.clear();
  delay(1000);
}

void dump_switch(){
  Serial.print("SWITCH: ");
  Serial.println(curr_switch);
}
void dump_encoder(){
  Serial.print(", ENCODER: ");
  Serial.println(curr_enc);
}

void loop() {
  curr_switch = analogRead(A4);
  curr_enc = encoder.read();
  unsigned long timestamp = millis();
  if(timing) {
    unsigned long test = timestamp - start_time;
    if(test > duration){
      announce_time();
    } else {
      draw_time((duration - test) + 60000, timestamp % 500 < 250);
    }
  } else if(curr_switch > 1 && 
            curr_switch < 10){
    if(duration > 0){
      dump_switch();
      mark_start(timestamp);
    } else {
      dump_switch();
      display.setSegments(hello);
      delay(3000);
      display.clear();
    }
  } else if(curr_enc != prev_enc){
    duration = curr_enc * 60000;
    if(curr_enc > 120){
        duration = 7200000;
    }
    if(curr_enc < 0){
        duration = 60000;
    }
    dump_encoder();
    draw_time(duration, 1);
  }  
  prev_switch = curr_switch;
  prev_enc = curr_enc;
  delay(20);
}