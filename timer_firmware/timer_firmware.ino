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
    unsigned int index;
    unsigned int samples[4];
    unsigned int thresholds[4];
    unsigned int n_thresholds;

  public:
    ButtonChoice(unsigned int _pin):
      pin(_pin), index(0), n_thresholds(0){}

    void add_button(unsigned int thresh){
      thresholds[n_thresholds] = thresh;
      n_thresholds++;
      if(n_thresholds > 4){
        Serial.println("ERROR: Cannot allocate more than 4 buttons!");
      }
    }
    void sample(){
      samples[index] = analogRead(pin);
      //Serial.println(samples[index]);
      index++;
      index %= 4;
    }
    unsigned int get(){
      unsigned int i;
      unsigned int j;
      unsigned int hits[4] = {0, 0, 0, 0};
      for(j = 0; j < n_thresholds; j++){
        for(i = 0; i < 4; i++){        
           if(abs(samples[i] - thresholds[j]) < 10){
             hits[j]++;
           }
        }
        if(hits[j] > 3){
          return j + 1;
        }
      }
      return 0;
    }
};

volatile int curr_switch;
volatile int prev_switch;
unsigned long start_time = 0;
unsigned long duration = 0;
int state;
unsigned int button;
ButtonChoice buttons(A4);



float e_0 = 329.63/2;
float f_0 = 349.23/2;
float g_0 = 392.00/2;
float a_0 = 440.00/2;
float b_0 = 493.88/2;
float c_1 = 261.63;
float d_1 = 293.66;
float e_1 = 329.63;
float f_1 = 349.23;
float g_1 = 392.00;
float a_1 = 440.00;
float b_1 = 493.88;
float c_2 = 523.25;

struct {
  float pitch;
  unsigned int length;
} Note;


enum states {
  TIMING = 1,
  SLEEPING,
  SETTING
};



void note(float pitch, unsigned int length) {
  int i;
  unsigned int accum = 0;
  unsigned long musecs = 1000000 / pitch * 0.5;
  while(accum < length) {
    digitalWrite(8, HIGH);
    delayMicroseconds(musecs);
    digitalWrite(8, LOW);
    delayMicroseconds(musecs);
    accum += musecs/1000;
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

void bach_1() {
  note(g_0, 128); note(c_1, 64); note(d_1, 64); note(e_1, 128); note(e_1, 128);
  note(d_1, 128); note(f_1, 128); note(e_1, 128); note(g_0, 128); note(f_0, 128); 
  note(e_1, 64);  note(c_1, 64); note(d_1, 128); note(f_0, 128); note(e_0, 128);  
  note(b_0, 128); note(c_1, 128);
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
  bach_1();
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
  alarm();
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
  buttons.add_button(506);
  buttons.add_button(759);
}

void loop() {
  buttons.sample();
  //
  unsigned long timestamp = millis();
  int state = get_state();
  unsigned int selection = buttons.get();

  //dump_button(selection);
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
    } else if(selection == 2){ 
       duration -= 60000;
    } else if(selection == 3){ 
       mark_start();
    }
    if(duration < 60000 ){
      duration = 60000;
    }
    draw_time(duration, 1);
    delay(20);
    
  } else if(state == SLEEPING){
    if(button == 3){
      greet();
    }
  } 

  //delay(20);
}

