/*
for the SmartCar Shield v1.1, this has the TB6612FNG H-Bridge motor controller https://dronebotworkshop.com/tb6612fng-h-bridge/

*/
#define PWMA 5 //this pin controls speed of motor A (left) - analog value 30-255 only (min speed 30 from experimentation, anything below this does not deliver sufficient power to run all 4 motors)
#define PWMB 6 //this pin controls speed of motor B (right) - analog value 30-255 only

#define AIN1 7 //controls power to motor A --read the RED labels on the pinout on the circuit schematic for the shield. https://drive.google.com/file/d/17PbbtRU9EobVzNFHf3JRnfc0aW9sLZYT/view?usp=sharing 
//#define AIN2 111 //controls power to motor A --this is not connected to the UNO directly, instead is signal AIN1 passed through a 7414 Schmitt inverter.

#define BIN1 8 //controls power to motor B
//#define BIN2 1000 //controls power to motor B - BIN1 passed through 7414 Schmitt inverter.

#define STBY 3

int MotorSpeed =90; //fixed motor speed for now - write this to PWM. will later have this variable rewritten using a controller. If A and B have different speeds, the car will turn.

#define trig 13 //define ultrasonic trig pin on the arduino
#define echo 12 //the echo pin on the ultrasonic sensor

float distance;
float pulse_time;
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

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
digitalWrite(STBY,HIGH); //forums suggest STBY on the H-bridge needs to be held HIGH for the bridge to work. Testing shows this to be the case.
pinMode(trig,OUTPUT);
pinMode(echo,INPUT);
delay(5000); //just so the car doesnt start immediately with the USB cable still connected.
}

void loop() {
  // put your main code here, to run repeatedly:
digitalWrite(trig,LOW);
delayMicroseconds(2); //this guarantees no signal is currently being sent
digitalWrite(trig,HIGH);
delayMicroseconds(10); //send a 10us long burst of ultrasonic pulses -- this can be higher or lower, but 10us seems to work best
digitalWrite(trig,LOW);

pulse_time = pulseIn(echo,HIGH); //look for echo to run high - counts in us

distance = 0.5*pulse_time*0.0343;

if (distance>15){ //while distance is >5cm, the car will continue moving forward
analogWrite(PWMA,MotorSpeed);
analogWrite(PWMB,MotorSpeed);
digitalWrite(AIN1,HIGH);
digitalWrite(BIN1,HIGH);
delay(100);
Serial.println(distance);
}

if (distance<15){
analogWrite(PWMA,0); //the best solution I have found to stop the car so far is using PWM=0, essentially just not delivering power to the motors at all.
analogWrite(PWMB,0);
digitalWrite(AIN1,HIGH);
digitalWrite(BIN1,LOW);
analogWrite(PWMA,75);
analogWrite(PWMB,75);
delay(200); //the car will turn for 150ms, then move in straight line if it can
}

}
