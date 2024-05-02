
// --- Global Variables ---
// period : stores the value of the output wave period
volatile float colorSensorPeriod;

// timer: stores the value of TIMER1
volatile unsigned int timer1Value = 0;

int PIN_COLOR_SENSOR = 0b00000100; // pin 10 (color sensor)
int PIN_QTI_LEFT = 0b100000;       // 13
int PIN_QTI_RIGHT = 0b000001;      // 8

int MAX_BLUE_PERIOD = 502;
int MIN_BLUE_PERIOD = 325;
int MAX_YELLOW_PERIOD = 128;
int MIN_YELLOW_PERIOD = 35;

// timer 0 with 1024 prescaler will count 0.0164 seconds, for 1 minute we need
int TIME_TO_RETURN_IN_SECONDS = 60;
bool timeReached = false;
volatile int overFlowCounter = 0;

// Colors
int YELLOW = 0;
int BLUE = 1;

// States
int homeColor;
bool leftOpponent;
bool madeTripAcrossOpponent;
int numberOfTrips;

void forward()
{
  DDRD = 0b01010000;  // pin 6 (left) forward and 4 forward(right)
  PORTD = 0b01010000; // set to high to move forward (use PWM to control speed)
}

void backward()
{
  DDRD = 0b10001000; // pin 7 (left) backward pin 3 backward (right)
  PORTD = 0b10001000;
}

void turnRight()
{
  // pin6 forward and pin 3 backward
  DDRD = 0b01001000;
  PORTD = 0b01001000;
}

void turnLeft()
{
  // pin 4 forward and pin 7 backward
  DDRD = 0b10010000;
  PORTD = 0b10010000;
}

// Interrupt service routine (color sensor)
ISR(PCINT0_vect)
{
  // resets the timer to zero on a rising edge,
  if (PINB & PIN_COLOR_SENSOR)
  {
    TCNT1 = 0;
  }
  else
  {
    timer1Value = TCNT1;
  }
}

// Inteerrupt service routine (timer 0)
ISR(TIMER0_OVF_vect)
{
  overFlowCounter++;
  if ((overFlowCounter * 0.0164) >= TIME_TO_RETURN_IN_SECONDS)
  {
    timeReached = true;
  }
}

void initColor()
{
  // Initialize interrupts
  PCICR |= 0b00000001; // Enable PCIE0

  // Initialize timer
  TCCR1A = 0b00000000; // Normal Mode
  TCCR1B = 0b00000001; // 1 pre-scalar
  TCNT1 = 0;           // reset timer
}

int getColor()
{

  PCMSK0 |= PIN_COLOR_SENSOR; // Enable pin change interrupt for pin 10

  // short delay (5 ~10 milliseconds)

  _delay_ms(5);

  PCMSK0 &= ~PIN_COLOR_SENSOR; // Disable pin change interrupt for pin 10

  colorSensorPeriod = timer1Value * 0.0625 * 2; // in microseconds
  if (colorSensorPeriod >= 200)                 // this could also be black btw
  {
    return BLUE;
  }
  else if (colorSensorPeriod <= 150)
  {
    return YELLOW;
  }
}

void initCountDownTimer()
{

  TCCR0A = 0b00000000; // Normal Mode
  TCCR0B = 0b00001101; // 1024 pre-scalar
  TIMSK0 = 0b00000001;
  TCNT0 = 0; // reset timer
}

int main(void)
{

  // Initialize color sensor
  DDRB = 0b00000000; // ALL pins on B as inputs.
  Serial.begin(9600);
  initColor();
  initCountDownTimer();
  sei();
  forward();
  _delay_ms(400);
  homeColor = getColor();

  while (1)
  {
    int currentColor = getColor();
    bool edge_left = PINB & PIN_QTI_LEFT;
    bool edge_right = PINB & PIN_QTI_RIGHT;

    if (timeReached)
    {
      DDRD = 0b00000000;
      break;
    }

    if (edge_left && edge_right)
    {
      backward();
      _delay_ms(500);
      // Serial.println("both black");
      turnLeft(); // this used to be turn right
      _delay_ms(500);
    }
    else if (edge_left && !edge_right)
    {
      // Serial.println("edge left and !edge right");
      turnRight();
      _delay_ms(20);
    }
    else if (!edge_left && edge_right)
    {
      // Serial.println("!edge left and edge right");
      turnLeft();
      _delay_ms(20);
    }
    else
    {
      if ((currentColor == homeColor) && !leftOpponent)
      {
        forward();
      }
      else if ((currentColor == homeColor) && leftOpponent)
      {
        forward();
        _delay_ms(1250);
        backward();
        _delay_ms(850);
        if (numberOfTrips % 2 == 0)
        {
          turnLeft();
          _delay_ms(1200);
        }
        else
        {
          turnRight();
          _delay_ms(1250);
        }

        leftOpponent = false;
        madeTripAcrossOpponent = false;
        numberOfTrips++;
      }
      else if ((currentColor != homeColor) && !madeTripAcrossOpponent)
      {
        leftOpponent = true;
        forward();
        _delay_ms(350);
        if (numberOfTrips % 2 == 0)
        {
          turnLeft();
          _delay_ms(580);
        }
        else
        {
          turnRight();
          _delay_ms(580);
        }
        forward();
        _delay_ms(4800);
        if (numberOfTrips % 2 == 0)
        {
          turnLeft();
          _delay_ms(500);
        }
        else
        {
          turnRight();
          _delay_ms(500);
        }
        forward();
        _delay_ms(500);
        madeTripAcrossOpponent = true;
      }
      else
      {
        forward();
      }
    }
  }
}
