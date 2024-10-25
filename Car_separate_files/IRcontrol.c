#include "carMain.h"

void IR_setup(){
  irrecv.enableIRIn(); //enable the IR detection
  irrecv.blink13(true); //this will flash the built in LED to show a signal is received
}

void IR_control(){
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
      case 0x9F1D53AD: //help (reset speed)
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