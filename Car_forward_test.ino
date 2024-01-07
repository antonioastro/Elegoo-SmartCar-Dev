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

int MotorSpeed =125; //fixed motor speed for now - write this to PWM. will later have this variable rewritten using a controller. If A and B have different speeds, the car will turn.

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
LEFT    LOW       HIGH
RIGHT   HIGH      LOW

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
}

void loop() {
  // put your main code here, to run repeatedly:
delay(5000); //just so the car doesnt start immediately with the wire still connected.
analogWrite(PWMA,MotorSpeed);
analogWrite(PWMB,MotorSpeed);
digitalWrite(AIN1,HIGH);
digitalWrite(BIN1,HIGH);
delay(1000);
analogWrite(PWMA,0); //the best solution I have found to stop the car so far is using PWM=0, essentially just not delivering power to the motors at all.
analogWrite(PWMB,0);
}
