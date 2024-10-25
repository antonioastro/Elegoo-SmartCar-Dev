#include <FastLED.h>
#include <Servo.h>
#include <IRremote.h>

void mode_switch();
void direction_check();
void increase_speed();
void decrease_speed();
void forward();
void turn_left();
void turn_right();
void reverse();
void stop();
void rotate90left();
void rotate90right();
void backup();

void flash_led_red();
void make_led_yellow();
void make_led_pink();
void cosmetic_setup();

void IR_control();
void IR_setup();

void autoPilot();
void avoidance();
void distance_check();
void autopilot_setup();

#define PWMA 5 //this pin controls speed of motor A (left) - analog value 30-255 only (min speed 30 from experimentation, anything below this does not deliver sufficient power to run all 4 motors)
#define PWMB 6 //this pin controls speed of motor B (right) - analog value 30-255 only
#define AIN1 7 //controls power to motor A --read the RED labels on the pinout on the circuit schematic for the shield. https://drive.google.com/file/d/17PbbtRU9EobVzNFHf3JRnfc0aW9sLZYT/view?usp=sharing 
#define BIN1 8 //controls power to motor B
#define STBY 3 //standby pin
#define trig 13 //define ultrasonic trig pin on the arduino
#define echo 12 //the echo pin on the ultrasonic sensor
#define servo_pin 10 //define pin connection for the servo
#define IR 9 //define the IR receiving pin
#define mode_pin 2 //define the interrupt pin - mode switcher
#define RGB_pin 4  //the RGB LED is connected to pin 4 of the Arduino
#define numLEDs 1 //on the shield there is only 1 LED.

extern float distance;
extern float pulse_time;
extern int hit;
extern float pos;
extern int mode_number;
extern float MotorSpeed;
extern float direction;
extern float TurnSpeed;
extern int led_red_loop;
extern struct CRGB leds;
extern struct decode_results results;

extern Servo scanner;
extern struct IRrecv irrecv; 