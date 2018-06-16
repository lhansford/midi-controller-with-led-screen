 // Adapted from http://bildr.org/2011/02/cd74hc4067-arduino/

#include <LiquidCrystal.h>
#include "MIDIUSB.h"

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


// Mux control pins
int s0 = 10;
int s1 = 11;
int s2 = 12;
int s3 = 13;

//Mux in “SIG” pin
int SIG_pin = 0;

int ACTIVE_POTS = 16;
int MIDI_CHANNEL = 1;
float MIDI_DIVISOR = 8.06299;

int LAST_CHANNEL_VALUES[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Takes a pot value and converts it to a midi cc value
int getMIDIValue(int potValue) {
  return potValue / MIDI_DIVISOR;
}

void updateValues() {
  bool updatedScreen = false;
  for(int i = 0; i < ACTIVE_POTS; i ++){
    int newValue = getMIDIValue(readMux(i));
    
    if (!updatedScreen && newValue != LAST_CHANNEL_VALUES[i] && newValue != LAST_CHANNEL_VALUES[i] - 1 && newValue != LAST_CHANNEL_VALUES[i] + 1) { // The plus/minus 1 prevents update on minor fluctuations.
      updatedScreen = true;
      updateScreen(i, newValue);
      LAST_CHANNEL_VALUES[i] = newValue;
      updateMIDI(i, newValue);
    }
//    Serial.println("===");
//    Serial.println(i);
//    Serial.println(LAST_CHANNEL_VALUES[i]);
  }
  MidiUSB.flush();
}

void updateScreen(int channel, int value) {
  String a = "Channel ";
  String firstLine = a + channel;
  lcd.clear();
  lcd.print(firstLine);
  lcd.setCursor(0, 1);
  lcd.print(value);
}

void updateMIDI(int control, int value) {
  midiEventPacket_t event = {0x0B, 0xB0 | MIDI_CHANNEL, control, value};
  MidiUSB.sendMIDI(event);
}

int readMux(int channel) {
  int controlPin[] = {s0, s1, s2, s3};
  int muxChannel[16][4]={
    {0,0,0,0}, //channel 0
    {0,0,0,1}, //channel 8
    {0,0,1,0}, //channel 4
    {0,0,1,1}, //channel 12
    {1,0,0,0}, //channel 1
    {1,0,0,1}, //channel 9
    {1,0,1,0}, //channel 5
    {1,0,1,1}, //channel 13
    {0,1,0,0}, //channel 2
    {0,1,0,1}, //channel 10
    {0,1,1,0}, //channel 6
    {0,1,1,1}, //channel 14
    {1,1,0,0}, //channel 3
    {1,1,0,1}, //channel 11
    {1,1,1,0}, //channel 7
    {1,1,1,1} //channel 15
   };
   //loop through the 4 sig
   for(int i = 0; i < 4; i ++){
    digitalWrite(controlPin[i], muxChannel[channel][i]);
   }
   //read the value at the SIG pin
   int val = analogRead(SIG_pin);
   //return the value
   return val;
}

void setup() {
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  
  digitalWrite(s0, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("MIDI Controller");
  
  Serial.begin(9600);
//  Serial.println("Startup");
}

void loop() {
//  Serial.println("Loop");
  updateValues();
  delay(100);
}

