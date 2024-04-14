
void forward(){

  DDRD = 0b01010000;    // pin 6 (left) forward and 4 forward(right)
  PORTD = 0b01010000; // set to high to move forward (use PWM to control speed)
  // note will adjust how long delay to account for distance
  //DDRD = 0b00000000;
} 

void backward(){
  DDRD = 0b10001000; // pin 7 (left) backward pin 3 backward (right)
  PORTD = 0b10001000;

  //DDRD = 0b00000000;
}

void turnRight(){

  // pin6 forward and pin 3 backward
  DDRD = 0b01001000;
  PORTD = 0b01001000;

  //DDRD = 0b00000000;
}

void turnLeft(){

    // pin 4 forward and pin 7 backward
  DDRD = 0b10010000;
  PORTD = 0b10010000;

 // DDRD = 0b00000000;

}



int main(void){
  
  forward();
  _delay_ms(5000);

  turnRight();
  _delay_ms(750);

  forward();
  _delay_ms(5000);

  turnLeft();
  _delay_ms(750);

  forward();
  _delay_ms(2500);

  backward();
  _delay_ms(7500);

  turnLeft();
  _delay_ms(750);

  forward();
  _delay_ms(5000);

  turnRight();
  _delay_ms(750);

  DDRD = 0b00000000; /
