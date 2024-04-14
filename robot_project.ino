void forward(){
  DDRD = 0b01010000;    // pin 6 (left) forward and 4 forward(right)
  PORTD = 0b01010000; // set to high to move forward (use PWM to control speed)
} 

void backward(){
  DDRD = 0b10001000; // pin 7 (left) backward pin 3 backward (right)
  PORTD = 0b10001000;
}

void turnRight(){
  // pin6 forward and pin 3 backward
  DDRD = 0b01001000;
  PORTD = 0b01001000;
}

void turnLeft(){
    // pin 4 forward and pin 7 backward
  DDRD = 0b10010000;
  PORTD = 0b10010000;
}

// period : stores the value of the output wave period
volatile float preiod;

// timer: stores the value of TIMER1
volatile unsigned int timer1Value = 0;

ISR(PCINT0_vect)
{
  // resets the timer to zero on a rising edge,
  if (PINB & 0b00000100)
  {
    TCNT1 = 0;
  }
  else
  {
    timer1Value = TCNT1;
  }
}

void initColor(){
  DDRB = 0b00000000; // pins 11 and 12 as outputs
}



int main(void){
  
  forward();
  _delay_ms(2000);

  turnRight();
  _delay_ms(750);

  forward();
  _delay_ms(2000);

  turnLeft();
  _delay_ms(750);

  forward();
  _delay_ms(1000);

  backward();
  _delay_ms(3000);

  turnLeft();
  _delay_ms(750);

  forward();
  _delay_ms(2000);

  turnRight();
  _delay_ms(750);

  DDRD = 0b00000000; 
