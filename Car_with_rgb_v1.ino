/*
for the SmartCar Shield v1.1, this has the TB6612FNG H-Bridge motor controller https://dronebotworkshop.com/tb6612fng-h-bridge/
*/

#define PWMA 5 //this pin controls speed of motor A (left) - analog value 30-255 only (min speed 30 from experimentation, anything below this does not deliver sufficient power to run all 4 motors)
#define PWMB 6 //this pin controls speed of motor B (right) - analog value 30-255 only
#define AIN1 7 //controls power to motor A --read the RED labels on the pinout on the circuit schematic for the shield. https://drive.google.com/file/d/17PbbtRU9EobVzNFHf3JRnfc0aW9sLZYT/view?usp=sharing 
#define BIN1 8 //controls power to motor B
#define STBY 3 //standby pin

/* 
---MOTOR SIDES---
LEFT  Motors A. FRONT M1, REAR M4
RIGHT Motors B. FRONT M2, REAR M3 --note that FRONT and REAR motors cannot be controlled individually.

---CONTROLLING THE MOTORS--- both motors A and B have 2 inputs.
IN1 	IN2 	Mode
HIGH  HIGH 	Short Brake --since IN2 is IN1 passed through Schmitt, I am not sure how to get these first 2 motor modes...
LOW  	LOW 	Stop ^
HIGH  LOW   Clockwise
LOW 	HIGH 	Counterclockwise

---DIRECTIONALS---
Dir     Motor A   Motor B
FORWARD HIGH      HIGH
REVERSE LOW       LOW
LEFT    HIGH      LOW
RIGHT   LOW       HIGH

---MOTOR SPEEDS (on carpet)---
Experiment, run the car for 1000ms, then pause. Measure distance in meters.

PWM value   Speed m/s 
30          0.06 
50          0.13
100         0.29  
125         0.37
150         0.47 
200         0.63
255         0.81
*/ 

/*Set up ultrasonics*/
#define trig 13 //define ultrasonic trig pin on the arduino
#define echo 12 //the echo pin on the ultrasonic sensor
float distance;
float pulse_time;
int hit = 20; //minimum distance to avoid collision in cm
int led_red_loop = 0; //counter for the flash_led_red loop

/*set up Servo*/
#include <Servo.h>
#define servo_pin 10 //define pin connection for the servo
Servo scanner; //create a named servo called scanner
float pos=40; //create a variable that can be adjusted to be the angle of the servo 0-180 only. 0-90 = right side, 90-180 = left side

/*set up IR control*/
#include <IRremote.h>
#define IR 9 //define the IR receiving pin
IRrecv irrecv(IR); 
decode_results results; //create an instance of the remote information decoded

/*set up mode checker*/
int mode_number = 0; //mode 0 = default (IR), mode 1 = autopilot, mode 2 = reset to 0.
#define mode_pin 2

/*set up variable speed*/
float MotorSpeed = 75; //default speed is 75 - on autopilot it will use this speed, and the speed can be adjusted between 55 and 255 from here in increments of 20.
float direction; //this variable will store the previous direction of the car when changing speed, so it will smoothly transition from one speed to another.
float TurnSpeed;

/*Hex Keys for remote control - current Sky TV Remote:
UP	 	      8A774570	//forwards
LEFT 	    	AC390F87	//left
RIGHT 		  AB390DF4	//right
DOWN		    1D407D2D	//reverse
CHANNEL_UP 	B6C3F7B0	//SpeedUp
CHANNEL_DWN	AACAD56D	//SpeedDown
PAUSE		    C2ECF552
PLAY		    D621733
NUMPAD_1	  4A19F6EF
NUMPAD_2	  211777CB
NUMPAD_3	  2217795E
NUMPAD_4	  9E87E040
NUMPAD_5	  62850C18
NUMPAD_6	  E3D3C987
NUMPAD_7	  E2D3C7F2
NUMPAD_8	  8B13B370
NUMPAD_9	  79FBB2D
NUMPAD_0	  BB2AC05E
SELECT		  70AF0C34	//stop
ARROWS>>	  661B0FE   //camera pan RIGHT - not set up yet, but intended for this.
ARROWS<<	  22C77576  //camera pan LEFT
BACKUP		  F24BFC4   //reverse for 1 second and turn in a random direction - not integrated yet,
HELP		    9F1D53AD	//RESET SPEED*/

/*set up RGB LED*/
#define RGB_pin 4  //the RGB LED is connected to pin 4 of the Arduino
#include <FastLED.h>
#define numLEDs 1 //on the shield there is only 1 LED.
CRGB leds[numLEDs]; //you can change the number of LEDs here, if using a WS2812 with multiple LEDs - i.e an LED strip array. must be of the form CRGB leds[float]

void setup() { //runs once at start-up
  Serial.begin(9600);
  digitalWrite(STBY,HIGH); //forums suggest STBY on the H-bridge needs to be held HIGH for the bridge to work. Testing shows this to be the case.

  pinMode(trig,OUTPUT); //define the ultrasonic pins as outout (trigger) and input (echo)
  pinMode(echo,INPUT);

  scanner.attach(servo_pin); //attach the servo to the arduino

  irrecv.enableIRIn(); //enable the IR detection
  irrecv.blink13(true); //this will flash the built in LED to show a signal is received

  pinMode(mode_pin,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(mode_pin),mode_switch,FALLING);

  pinMode(RGB_pin,OUTPUT);
  FastLED.addLeds<WS2812, RGB_pin, GRB>(leds,numLEDs);
}

void loop() { //repeats continuously
  switch(mode_number){
    case 0:
      IR_control(); //PINK
      break;
    case 1: 
      autopilot(); //YELLOW
      break;
  }
}

/* main functions for control of the car defined below */

void mode_switch(){
  stop();
  if (mode_number<1){
    mode_number++;
  } else if (mode_number==1){
    mode_number=0;
  }
}

void IR_control(){
  scanner.write(90);
  make_led_pink();
  
  if(irrecv.decode(&results)){ //check to see if an IR signal is received
    switch(results.value){ //do something depending on the "HEX value" of the key pressed
      case 0x8A774570: //intitially tested these on the HEX values, then converted to DEC - the same effect, so it doesn't matter if HEX or DEC. Needs changing for each remote used.
        forward(); //Up
        direction=0; //write direction as "0" for FORWARD
        break;
      case 0x1D407D2D: //Down
        reverse();
        direction=1; //write direction as "1" for REVERSE
        break;
      case 0xAC390F87: //Left
        turn_left();
        direction=3; //write direction as "3" for LEFT
        break;
      case 0xAB390DF4: //Right
        turn_right();
        direction=4; //write direction as "4" for RIGHT
        break;
      case 0x70AF0C34: //Select
        stop();
        direction=2; //write direction as "2" for STOP
        break;
      case 0xB6C3F7B0: //Channel_Up
        increase_speed();
        direction_check();
        break;
      case 0xAACAD56D: //Channel_Down
        decrease_speed();
        direction_check();
        break;
      case 0x9F1D53AD: //
        MotorSpeed=75;
        direction_check();
        break;
      case 0xF24BFC4: //back_up
        backup();
        break;
    }
  irrecv.resume(); //have to resume checking for IR inputs
  }
}

void autopilot(){
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

/* CORE FUNCTIONS BELOW */

void direction_check(){ //check the direction float - for a smooth transition between speeds
  if(direction==0){
    forward();
  }
  if(direction==1){
    reverse();
  }
  if(direction==2){
    stop();
  }
  if(direction==3){
    turn_left();
  }
  if(direction==4){
    turn_right();
  }
}

void increase_speed(){
  if (MotorSpeed<255){
    MotorSpeed=MotorSpeed+20;
  }
}

void decrease_speed(){
  if (MotorSpeed>55){
   MotorSpeed=MotorSpeed-20;
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

void forward(){
  analogWrite(PWMA,MotorSpeed);
  analogWrite(PWMB,MotorSpeed);
  digitalWrite(AIN1,HIGH);
  digitalWrite(BIN1,HIGH);
}

void turn_left(){
  if (MotorSpeed>115){
    TurnSpeed=115;
  }
  else{
    TurnSpeed=MotorSpeed;
  }
  digitalWrite(AIN1,HIGH);
  digitalWrite(BIN1,LOW);
  analogWrite(PWMA,TurnSpeed);
  analogWrite(PWMB,TurnSpeed);
}
 
void turn_right(){ 
  if (MotorSpeed>115){
    TurnSpeed=115;
  }
  else{
    TurnSpeed=MotorSpeed;
  }
  digitalWrite(AIN1,LOW);
  digitalWrite(BIN1,HIGH);
  analogWrite(PWMA,TurnSpeed);
  analogWrite(PWMB,TurnSpeed);
}

void reverse(){
  digitalWrite(AIN1,LOW);
  digitalWrite(BIN1,LOW);
  analogWrite(PWMA,MotorSpeed);
  analogWrite(PWMB,MotorSpeed);
}

void stop(){
  analogWrite(PWMA,0); //power to motors =0
  analogWrite(PWMB,0);
}

void rotate90left(){ //calibrated to rotate the car exactly 90 degrees to the left when MotorSpeed is 75 - this is only used on AutoPilot mode
  turn_left();
  delay(1000);
  stop();
}

void rotate90right(){ //calibrated to rotate the car exactly 90 degees to the right
  turn_right();
  delay(1000);
  stop();
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

void backup(){ //a clone of the avoidance procedure from the AutoPilot mode, but available in IR_Remote_Control
  reverse();
  delay(1000);
  if (random(1,10)<=5){
    rotate90left();
   }
   else{
    rotate90right();
  } 
}

/*cosmetic functions*/
void flash_led_red(){ //this is configured to run approx 1000ms.
  leds[0]= CRGB::Red; //write the colour to a certain LED in the chain. Can change 0 to be any number if you have an array
  for (led_red_loop=0;led_red_loop<=5; led_red_loop++){
    FastLED.setBrightness(0);
    FastLED.show(); //instructs the WS2812 to write the changes to the LEDs
    delay(200);
    FastLED.setBrightness(255);
    FastLED.show(); //instructs the WS2812 to write the changes to the LEDs
    delay(200);
  }
}

void make_led_yellow(){
  leds[0]= CRGB::Yellow; //write the colour to a certain LED in the chain. Can use CRGB(FF,FF,FF) instead, but the CRGB has pre-defined colours which is easier.
  FastLED.show(); //instructs the WS2812 to write the changes to the LEDs
}

void make_led_pink(){
  leds[0]= CRGB::DeepPink; //write the colour to a certain LED in the chain. Can use CRGB(FF,FF,FF) instead, but the CRGB has pre-defined colours which is easier.
  FastLED.show(); //instructs the WS2812 to write the changes to the LEDs
}