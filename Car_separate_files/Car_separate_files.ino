#include "carMain.h"

void setup() { //runs once at start-up
  Serial.begin(9600);
  digitalWrite(STBY,HIGH); //forums suggest STBY on the H-bridge needs to be held HIGH for the bridge to work. Testing shows this to be the case.

  autopilot_setup(); //function defined inside of autopilot.c
  IR_setup(); //funcion defined in IRcontrol.c
  cosmetic_setup(); //function defined in carCosmetics.c

  pinMode(mode_pin,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(mode_pin),mode_switch,FALLING);
}

void loop() { //repeats continuously
  switch(mode_number){
    case 0:
      IR_control(); //PINK
      break;
    case 1: 
      autoPilot(); //YELLOW
      break;
  }
}