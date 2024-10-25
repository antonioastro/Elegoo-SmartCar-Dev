/*
for the SmartCar Shield v1.1, this has the TB6612FNG H-Bridge motor controller https://dronebotworkshop.com/tb6612fng-h-bridge/
*/

#define PWMA 5 //this pin controls speed of motor A (left) - analog value 30-255 only (min speed 30 from experimentation, anything below this does not deliver sufficient power to run all 4 motors)
#define PWMB 6 //this pin controls speed of motor B (right) - analog value 30-255 only
#define AIN1 7 //controls power to motor A --read the RED labels on the pinout on the circuit schematic for the shield. https://drive.google.com/file/d/17PbbtRU9EobVzNFHf3JRnfc0aW9sLZYT/view?usp=sharing 
#define BIN1 8 //controls power to motor B
#define STBY 3 //standby pin
float MotorSpeed =70; //fixed motor speed for now - write this to PWM. will later have this variable rewritten using a controller. If A and B have different speeds, the car will turn.

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

#define trig 13 //define ultrasonic trig pin on the arduino
#define echo 12 //the echo pin on the ultrasonic sensor
float distance;
float pulse_time;
int hit = 20; //minimum distance to avoid collision in cm

#include <Servo.h>
#define servo_pin 10 //define pin connection for the servo
Servo scanner; //create a named servo called scanner
float pos=40; //create a variable that can be adjusted to be the angle of the servo 0-180 only. 0-90 = right side, 90-180 = left side

void setup() {
  Serial.begin(9600);
  digitalWrite(STBY,HIGH); //forums suggest STBY on the H-bridge needs to be held HIGH for the bridge to work. Testing shows this to be the case.

  pinMode(trig,OUTPUT); //define the ultrasonic pins as outout (trigger) and input (echo)
  pinMode(echo,INPUT);

  scanner.attach(servo_pin); //attach the servo to the arduino
  delay(5000); //just so the car doesnt start immediately with the USB cable still connected.
}

void loop() {
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

/* main functions for motion of the car defined below */

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
  digitalWrite(AIN1,HIGH);
  digitalWrite(BIN1,LOW);
  analogWrite(PWMA,75);
  analogWrite(PWMB,75);
}
 
void turn_right(){ 
  digitalWrite(AIN1,LOW);
  digitalWrite(BIN1,HIGH);
  analogWrite(PWMA,75);
  analogWrite(PWMB,75);
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

void rotate90left(){ //calibrated to rotate the car exactly 90 degrees to the left
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
    delay(1000);
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
  }
}
