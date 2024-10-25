#include "carMain.h"

void mode_switch(){
  stop();
  if (mode_number<1){
    mode_number++;
  } else if (mode_number==1){
    mode_number=0;
  }
}

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
  flash_led_red();
  //delay(1000);
  stop();
}

void rotate90right(){ //calibrated to rotate the car exactly 90 degees to the right
  turn_right();
  flash_led_red();
  //delay(1000);
  stop();
}

void backup(){ //a clone of the avoidance procedure from the AutoPilot mode, but available in IR_Remote_Control
  reverse();
  delay(1000);
  if (rand()%10<=5){
    rotate90left();
   }
   else{
    rotate90right();
  }
}