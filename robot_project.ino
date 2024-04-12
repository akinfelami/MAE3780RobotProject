
void forward(){

  DDRD = 0b01010000;    // pin 6 (left) forward and 4 forward(right)
  PORTD = 0b01010000; // set to high to move forward (use PWM to control speed)
  _delay_ms(1000); // note will adjust how long delay to account for distance
  DDRD = 0b00000000;
} 

void backward(){
  DDRD = 0b10001000; // pin 7 (left) backward pin 3 backward (right)
  PORTD = 0b10001000;
  _delay_ms(1000);
  DDRD = 0b00000000;
}

void turnRight(){

  // pin6 forward and pin 3 backward
  DDRD = 0b01001000;
  PORTD = 0b01001000;
  _delay_ms(1000);
  DDRD = 0b00000000;
}

void turnLeft(){

    // pin 4 forward and pin 7 backward
  DDRD = 0b10010000;
  PORTD = 0b10010000;
  _delay_ms(1000);
  DDRD = 0b00000000;

}



int main(void){
  
  forward()
  turnRight()
  forward()

  turnLeft()
  forward()

  backward()


}
