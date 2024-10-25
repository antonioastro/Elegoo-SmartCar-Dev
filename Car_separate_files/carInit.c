#include "carMain.h"

/*Set-up of the car*/
float distance;
float pulse_time;
int hit = 20; //minimum distance to avoid collision in cm

Servo scanner; //create a named servo called scanner
float pos=40; //create a variable that can be adjusted to be the angle of the servo 0-180 only. 0-90 = right side, 90-180 = left side

IRrecv irrecv(IR); 
struct decode_results results; //create an instance of the remote information decoded

int mode_number = 0; //mode 0 = default (IR), mode 1 = autopilot, mode 2 = reset to 0.

float MotorSpeed = 75; //default speed is 75 - on autopilot it will use this speed, and the speed can be adjusted between 55 and 255 from here in increments of 20.
float direction; //this variable will store the previous direction of the car when changing speed, so it will smoothly transition from one speed to another.
float TurnSpeed;

struct CRGB leds[numLEDs]; //you can change the number of LEDs here, if using a WS2812 with multiple LEDs - i.e an LED strip array. must be of the form CRGB leds[float]
int led_red_loop = 0; //counter for the flash_led_red loop