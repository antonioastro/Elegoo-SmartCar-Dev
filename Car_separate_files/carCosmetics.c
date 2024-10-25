#include "carMain.h"

void cosmetic_setup(){
  pinMode(RGB_pin,OUTPUT);
  FastLED.addLeds<WS2812, RGB_pin, GRB>(leds,numLEDs);
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

