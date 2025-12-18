/* Example code for TM1637 4 digit 7 segment display with Arduino. More info: www.www.makerguides.com */

// Include the library:
#include "TM1637Display.h"


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

class ButtonChoice {
  private:
    unsigned int pin;
    unsigned int thresholds[4];
    unsigned int n_thresholds;

  public:
    ButtonChoice(unsigned int _pin):
      pin(_pin), n_thresholds(1){
        thresholds[0] = 100;
      }

    void add_button(unsigned int thresh){
      thresholds[n_thresholds] = thresh;
      n_thresholds++;
      if(n_thresholds > 4){
        Serial.println("ERROR: Cannot allocate more than 4 buttons!");
      }
    }
    
    unsigned int get(){
      unsigned int sample = analogRead(pin);
      Serial.println(sample);
      unsigned int i = 0;
      for(i = 0; i < n_thresholds; i++){
         if(sample < thresholds[i]){
           return i;
         }
      }
      return i;
    }
};

volatile int curr_switch;
volatile int prev_switch;
unsigned long start_time = 0;
unsigned long duration = 0;
int state;
unsigned int button;
ButtonChoice buttons(A4);

const float e_0 = 329.63/2;
const float f_0 = 349.23/2;
const float g_0 = 392.00/2;
const float a_0 = 440.00/2;
const float b_0 = 493.88/2;
const float c_1 = 261.63;
const float d_1 = 293.66;
const float e_1 = 329.63;
const float f_1 = 349.23;
const float g_1 = 392.00;
const float a_1 = 440.00;
const float b_1 = 493.88;
const float c_2 = 523.25;

class Note {
  public:
   float pitch;
   unsigned long length;
   Note(float _p, unsigned long _l): pitch(_p), length(_l){} 
};


enum states {
  TIMING = 1,
  SLEEPING,
  SETTING
};


void melody(Note notes[], unsigned int count) {
  int i;
  unsigned long accum;
  unsigned long musecs;
  noInterrupts();
  for(i = 0; i < count; i++){ 
      accum = 0;     
      musecs = 1000000 / notes[i].pitch * 0.5;
      while(accum < notes[i].length * 1000) {
        digitalWrite(8, HIGH);
        delayMicroseconds(musecs);
        digitalWrite(8, LOW);
        delayMicroseconds(musecs);
        accum += musecs;
      }
  }
  interrupts();
}


void up_beep() {
  int i;
  for (i = 0; i < 30; i++) {
    digitalWrite(8, HIGH);
    delayMicroseconds(500);
    digitalWrite(8, LOW);
    delayMicroseconds(500);
  }
}

void down_beep() {
  int i;
  for (i = 0; i < 10; i++) {
    digitalWrite(8, HIGH);
    delayMicroseconds(2000);
    digitalWrite(8, LOW);
    delayMicroseconds(2000);
  }
}

Note bach_1[17] = {
  Note(g_0, 128), Note(c_1, 64), Note(d_1, 64), Note(e_1, 128), Note(e_1, 128),
  Note(d_1, 128), Note(f_1, 128), Note(e_1, 128), Note(g_0, 128), Note(f_0, 128), 
  Note(e_1, 64),  Note(c_1, 64), Note(d_1, 128), Note(f_0, 128), Note(e_0, 128),  
  Note(b_0, 128), Note(c_1, 128)
};

Note bach_2[23] = {
  Note(g_1, 128), Note(g_1, 256), Note(f_1, 64), Note(e_1, 64), Note(d_1, 64), 
  Note(c_1, 64), Note(d_1, 64), Note(c_1, 64), Note(b_0, 64),  Note(a_0, 64),
  Note(g_0, 128), Note(a_0, 64), Note(b_0, 64), Note(c_1, 64), Note(d_1, 64),
  Note(e_1, 64), Note(c_1, 64), Note(f_1, 64), Note(e_1, 64), Note(d_1, 64),
  Note(c_1, 64), Note(e_1, 128), Note(d_1, 128)
};

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

void mark_start(){
  start_time = millis();
  set_state(TIMING);
  alert();
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
  melody(bach_1, 17);
  delay(3000);
  display.clear();
  set_state(SETTING);
}

void sleep(){
  duration = 0;
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
  melody(bach_2, 23);
  sleep();
}

void dump_button(unsigned int val){
  Serial.print("BUTTON: ");
  Serial.println(val);
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
  greet();
  buttons.add_button(254);
  buttons.add_button(600);
  buttons.add_button(1024);
}

void loop() {
  //
  unsigned long timestamp = millis();
  int state = get_state();
  unsigned int selection = buttons.get();

  dump_button(selection);
  if(state == TIMING) {
    unsigned long test = timestamp - start_time;
    if(test > duration){
      announce_time();
    } else {
      draw_time((duration - test) + 60000, timestamp % 500 < 250);
    }
  } else if(state == SETTING){   
    if(selection == 1) {
       duration += 60000;
       up_beep();
       delay(100);
    } else if(selection == 2){ 
       duration -= 60000;
       down_beep();
       delay(100);
    } else if(selection == 3){ 
       mark_start();
    }
    if(duration < 60000 ){
      duration = 60000;
    }
    draw_time(duration, 1);
            
    
  } else if(state == SLEEPING){
    if(selection == 3){
      up_beep();
      greet();
    }
  } 

  //delay(20);
}

