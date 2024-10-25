#include "carMain.h"

void autopilot_setup(){
  pinMode(trig,OUTPUT); //define the ultrasonic pins as outout (trigger) and input (echo)
  pinMode(echo,INPUT);
  scanner.attach(servo_pin); //attach the servo to the arduino
}

void autoPilot(){
  MotorSpeed=75; //autopilot runs at a slow speed to ensure it functions correctly.
  for (pos=30; pos<=150; pos=pos+2){ //scan constantly between 30 and 150deg
    scanner.write(pos);
    distance_check();
    avoidance(); //check to see if an avoidance manouevure is needed
    delay(5); //a delay of 5ms is needed otherwise the servo does not have time to turn.
  }

  for (pos=150; pos>=30; pos=pos-2){ //scan constantly from 150 back to 30deg
    scanner.write(pos);
    distance_check();
    avoidance(); //check to see if an avoidance manouevure is needed - otherwise move forward. this is entirely embedded within this function
    delay(5);
  }
}

void avoidance(){
  if (distance<hit && pos==90){ //if obstacle is ahead, reverse slightly and turn left 90 degrees
    reverse();
    flash_led_red(); //the flash LED Red is a cosmetic change that flashes the LED Red to signify that a collision was about to occur and it is now running an avoidance manoevure.
    //delay(1000);
    if (random(1,10)<=5){
      rotate90left();
    }
    else{
      rotate90right();
    }
  }
  if (distance<hit && pos<90){ //turn left if obstacle is on the right
    turn_left(); 
  }
  if (distance<hit && pos>90){ //turn right if obstacle is on the left
    turn_right();
  }
  if (distance>hit){ //only move forward if there is no obstacle within range
    forward();
    make_led_yellow();
  }
}

void distance_check(){ 
  digitalWrite(trig,LOW);
  delayMicroseconds(2); //this guarantees no signal is currently being sent
  digitalWrite(trig,HIGH);
  delayMicroseconds(10); //send a 10us long burst of ultrasonic pulses -- this can be higher or lower, but 10us seems to work best
  digitalWrite(trig,LOW);

  pulse_time = pulseIn(echo,HIGH); //look for echo to run high - counts in us

  distance = 0.5*pulse_time*0.0343;
  return distance; //distance in cm
}