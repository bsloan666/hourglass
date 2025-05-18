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
volatile int curr_switch;
volatile int prev_switch;
unsigned long start_time = 0;
unsigned long duration = 0;
int state;
int button_pressed = 0;
float c_1 = 261.63;
float d_1 = 293.66;
float e_1 = 329.63;
float f_1 = 349.23;
float g_1 = 392.00;
float a_1 = 440.00;
float b_1 = 493.88;
float c_2 = 523.25;



enum states {
  TIMING = 1,
  SLEEPING,
  SETTING
};


Encoder encoder(2, 3);

void note(float note, unsigned int length) {
  int i;
  unsigned long musecs = 1000000 / note * 0.5;
  for (i = 0; i < length; i++) {
    digitalWrite(8, HIGH);
    delayMicroseconds(musecs);
    digitalWrite(8, LOW);
    delayMicroseconds(musecs);
  }
}


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

void scale() {
  note(c_1, 100);
  note(d_1, 100);
  note(e_1, 100);
  note(f_1, 100);
  note(g_1, 100);
  note(a_1, 100);
  note(b_1, 100);
  note(c_2, 400);
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
  set_state(TIMING);
  beep();
}

int get_state(){
   return state;
}

void set_state(int in_state){
  state = in_state;
  dump_state();
}
void greet(){
  delay(1000);
  display.setBrightness(0);
  display.setSegments(hello);
  int i;
  for(i = 0; i < 8; i++){
    display.setBrightness(i);
    delay(100);
  }
  scale();
  delay(3000);
  display.clear();
  set_state(SETTING);
}

void sleep(){
  duration = 0;
  encoder.readAndReset();
  display.clear();
  set_state(SLEEPING);
}
void cancel(){
  alert();
  display.setSegments(bye);
  delay(3000);
  sleep();
}

void announce_time(){
  duration = 0;
  display.setSegments(done);
  alarm();
  sleep();
}

void dump_switch(){
  Serial.print("SWITCH: ");
  Serial.println(curr_switch);
}
void dump_encoder(){
  Serial.print("ENCODER: ");
  Serial.println(curr_enc);
}
void dump_state(){
  Serial.print("STATE: ");
  if(state == TIMING){
    Serial.println("TIMING");
  } else if(state == SETTING){
    Serial.println("SETTING");
  } else if(state == SLEEPING){
    Serial.println("SLEEPING");
  }
}

void setup() {
  Serial.begin(9600);
  display.clear();
  pinMode(8, OUTPUT);
  pinMode(12, INPUT_PULLUP);
  PCICR = B00000001;
  PCMSK0 = B00010000;
  greet();
}

void loop() {
  curr_enc = encoder.read();
  unsigned long timestamp = millis();
  int state = get_state();
  if(state == TIMING) {
    unsigned long test = timestamp - start_time;
    if(test > duration){
      announce_time();
    } else {
      draw_time((duration - test) + 60000, timestamp % 500 < 250);
    }
  } else if(state == SETTING){
    if(curr_enc != prev_enc){
      duration = curr_enc * 60000;
      if(curr_enc > 120){
          duration = 7200000;
      }
      if(curr_enc < 0){
          duration = 60000;
      }
      dump_encoder();
      draw_time(duration, 1);
    } else if(button_pressed){
      mark_start(timestamp);
      button_pressed = 0;
      dump_switch();
    } 
  } else if(state == SLEEPING){
    if(button_pressed){
      greet();
      button_pressed = 0;
      dump_switch();
    }
  } 
  prev_switch = curr_switch;
  prev_enc = curr_enc;
  delay(20);
}

ISR (PCINT0_vect) {
  curr_switch = digitalRead(12);
  if(curr_switch != prev_switch){
    if(curr_switch < prev_switch){
      button_pressed = 1;
    }
  }
  prev_switch = curr_switch;
}