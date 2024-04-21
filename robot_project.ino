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
volatile float period;

// timer: stores the value of TIMER1
volatile unsigned int timer1Value = 0;

// Interrupt service routine

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
  // DDRB = 0b00000000; // ALL pins on B as inputs. 
  sei();

  // Initialize interrupts
  PCICR |= 0b00000001; // Enable PCIE0

  // Initialize timer
  TCCR1A = 0b00000000; // Normal Mode
  TCCR1B = 0b00000001; // 1 pre-scalar
  TCINT1 = 0; //reset timer
 
}

int getColor(){


  PCMSK0 |= 0b00000100; // Enable pin change interrupt for pin 10

  // short delay (5 ~10 milliseconds)

  _delay_ms(5);

  PCMSK0 &= ~0b00000100; // Disable pin change interrupt for pin 10

  period = timer1Value * 0.0625 * 2;

}


int main(void){


 // Initialize color sensor
 
//  Serial.begin(9600);
//  initColor();
//  sei();

 // while(1){
    //getColor();

    //Serial.println(period);
       
    forward();
    _delay_ms(2000);

    turnRight();
    _delay_ms(700);

    forward();
    _delay_ms(2000);

    turnLeft();
    _delay_ms(700);

    forward();
    _delay_ms(1000);

    backward();
    _delay_ms(3000);

    turnLeft();
    _delay_ms(700);

    forward();
    _delay_ms(2000);

    turnRight();
    _delay_ms(700);

    DDRD = 0b00000000; 

    _delay_ms(1000);
  

}
  
