
// --- Global Variables ---
// Stores the value of the color sensor period
volatile float colorSensorPeriod;

// stores the value of TIMER1 (used to calculate the color sensor period)
volatile unsigned int timer1Value = 0;

int PIN_COLOR_SENSOR = 0b00000100; // pin 10 (to read thecolor sensor)
int PIN_QTI_LEFT = 0b100000;       // 13 (to read the QTI left)
int PIN_QTI_RIGHT = 0b000001;      // 8 (to red the QTI right)

int MAX_BLUE_PERIOD = 502;   //  (max threshold for blue color detection, based on testing)
int MIN_BLUE_PERIOD = 325;   //   (min threshold for blue color detection, based on testing)
int MAX_YELLOW_PERIOD = 128; //  (max threshold for yellow color detection, based on testing)
int MIN_YELLOW_PERIOD = 35;  //   (min threshold for yellow color detection, based on testing)

int TIME_TO_RETURN_IN_SECONDS = 58; // timer 0 with 1024 prescaler will count 0.0164 seconds.
bool timeReached = false;           // set to true when the timer reaches the time set in TIME_TO_RETURN_IN_SECONDS

// timer 0 cannot count up to 1 minute. This is used to count the number of overflows of the timer0 has
// each overflow occurs after 0.0164 seconds (with a 1024 prescaler). mulyiplying the number of overflows
// by 0.0164 will give the number of seconds that have passed since the timer0 was reset.
volatile int overFlowCounter = 0;

// Color states - two states are possible: BLUE and YELLOW
int YELLOW = 0;
int BLUE = 1;

/**
 * More state variables. Boolean flags used to determine the state/position of the robot.
 * Will be used to determine the next action to take.
 * */
int homeColor;               // 1 for blue, 0 for yellow
bool leftOpponent;           // true if the robot has made it to the opponent side.
bool madeTripAcrossOpponent; // tue if robot has made a trip across the opponent side (to sweep blocks)
int numberOfTrips;           // number of trips made by the  robot

/** Moves the robot in the forward direction by using the motors on pins 6 and 4*/
void forward()
{
  DDRD = 0b01010000;  // pin 6 (left) forward and 4 forward(right)
  PORTD = 0b01010000; // set to high to move forward
}

/** Moves the robot in the backward direction by using the motors on pins 7 and 3*/
void backward()
{
  DDRD = 0b10001000;  // pin 7 (left) backward pin 3 backward (right)
  PORTD = 0b10001000; // set to high to move backward
}

/** Turns the robot to the left by using the motors on pins 6 and 3*/
void turnRight()
{
  DDRD = 0b01001000;  // pin 6 (left) forward and pin 3 (right) backward
  PORTD = 0b01001000; // set to high to turn right
}

/** Turns the robot to the right by using the motors on pins 4 and 7*/
void turnLeft()
{
  // pin 4 forward and pin 7 backward
  DDRD = 0b10010000;  // pin 7 backward (right) and pin 4 forward (left)
  PORTD = 0b10010000; // set to high to turn left
}

/** Interrupt service routine (color sensor)
 *  This function is called when the PIN_COLOR_SENSOR is triggered by the color senser
 *  It counts how long the timer takes from a rising edge to a falling egde and saved the value in timer1Value
 **/
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

/** Interrupt service routine (timer 0)
 *  This function is called when the timer0 overflows.
 *  It counts how many times the timer overflowed and saves the value in overFlowCounter
 *  It also checks if the timer has reached the time set in TIME_TO_RETURN_IN_SECONDS
 *  If it has, it sets the timeReached flag to true
 **/
ISR(TIMER0_OVF_vect)
{
  overFlowCounter++;
  if ((overFlowCounter * 0.0164) >= TIME_TO_RETURN_IN_SECONDS)
  {
    timeReached = true;
  }
}

/** Initializes the color sensor and timer
 *  Enables interrupts for PIN10( PCINT2 in arduino pinout)
 *  Initializes timer1 with 1 pre-scalar to count peroid of color sensor
 * */
void initColor()
{
  // Initialize interrupts
  PCICR |= 0b00000001; // Enable PCIE0

  // Initialize timer
  TCCR1A = 0b00000000; // Normal Mode
  TCCR1B = 0b00000001; // 1 pre-scalar
  TCNT1 = 0;           // reset timer
}

/** Calculates the color sensor period
 *  It reads the value of timer1Value and computes the period appropriately in microseconds
 *  It then uses threshold (based on testing) to determine if the color is blue or yellow
 *  If the color is blue, it returns BLUE (0)
 *  If the color is yellow, it returns YELLOW (1)
 */
int getColor()
{

  PCMSK0 |= PIN_COLOR_SENSOR; // Enable pin change interrupt for pin 10

  // short delay (5 ~10 milliseconds)

  _delay_ms(5);

  PCMSK0 &= ~PIN_COLOR_SENSOR; // Disable pin change interrupt for pin 10

  colorSensorPeriod = timer1Value * 0.0625 * 2; // in microseconds
  if (colorSensorPeriod >= 200)
  {
    return BLUE;
  }
  else if (colorSensorPeriod <= 150)
  {
    return YELLOW;
  }
}

/**
 * Initializes the interrupt and timer for the timer0 to use for the countdown.
 */

void initCountDownTimer()
{

  TCCR0A = 0b00000000; // Normal Mode
  TCCR0B = 0b00001101; // 1024 pre-scalar
  TIMSK0 = 0b00000001;
  TCNT0 = 0; // reset timer
}

/** Main function
 *  This function is called when the program starts
 */
int main(void)
{

  // Initialize color sensor
  DDRB = 0b00000000; // ALL pins on B as inputs.
  Serial.begin(9600); 

  // Initialize the interrupt and timer for color sensor
  initColor();

  // Initialize the interrupt and timer for timer0
  initCountDownTimer();

  // Enable global interrupts
  sei();

  // Move the robot forward ( to get past the black board so it doesn't confuse the QTIs)
  forward();
  _delay_ms(400);

  // Note the home Color
  homeColor = getColor();

  // Loop until the time is up
  while (1)
  { 
    // Read the color sensor
    int currentColor = getColor();

    // Read the state of th QTIs
    bool edge_left = PINB & PIN_QTI_LEFT;
    bool edge_right = PINB & PIN_QTI_RIGHT;


    // If timeReached is true, STOP the robot. 
    if (timeReached)
    {
      DDRD = 0b00000000;
      break;
    }

    if (edge_left && edge_right)
    // if both QTIs detect and edge, back up a little bit and turn left. 
    {
      
      backward();
      _delay_ms(500);
      turnLeft();
      _delay_ms(500);
    }
    else if (edge_left && !edge_right)
    // If we only detect a left edge, turn right to move away from the black board. 
    {
      // Serial.println("edge left and !edge right");
      turnRight();
      _delay_ms(20);
    }
    else if (!edge_left && edge_right)
    // if we only detect a right edge, turn left to move away from the black board.
    {
      // Serial.println("!edge left and edge right");
      turnLeft();
      _delay_ms(20);
    }
    else
    // BUT If we don't detect any edges: (note: checking for edges is the first thing we do)
    {
      if ((currentColor == homeColor) && !leftOpponent)
      // We are in the home posiiton, we have not made a trip to our oppponent side. (it could be that we are just starting the game)
      {
        forward(); // keep moving forward (edge detection will happen and adjust appropriately)
      }
      else if ((currentColor == homeColor) && leftOpponent)

      // We are back in the home position, but we have a trip to our opponent side. 
      // This means that we have some blocks in our possession, so we need to drop them home, and make another
      // trip to opponent side to sweep more blocks (time permitting)
      {
        forward();
        _delay_ms(1250);
        
        // Back up a little bit to make room for the blocks we are about to drop.
        backward();
        _delay_ms(850);

        // Turn around after making sufficient distance into home region.
        // Our robot starts out facing the right. This means that we need to turn left when we reach the opponent side to sweep blocks. 
        // However, once we make our first sweep (which goes across the board), and turn around, we will need to turn right to make the next sweep. 
        // numberOfTrips is used to keep track of how many trips we have made.
        // Since we start at 0, even number of counts means we need to turn left, and odd number means we need to turn right.

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

        // Once we have turned around, it is time to make another trip to the opponent side, so we reset the relevant flags. 
        // And increment the number of trips we have made.
        leftOpponent = false;
        madeTripAcrossOpponent = false;
        numberOfTrips++;
      }
      else if ((currentColor != homeColor) && !madeTripAcrossOpponent)

      // As soon as we detect a color other than home, we have made a trip to our opponent side.
      // Now we will sweep blocks to make the trip across the board.
      // If this is an even number of trips, we need to turn left to sweep blocks.
      // If it is an odd number of trips, we need to turn right to sweep blocks.
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
        // making the trip across the board. 
        forward();
        _delay_ms(4800);

        // Once across the board, we turn left/right depending on the number of trips we have made
        // to head back to the home position.
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
      // If all else fails, we just keep moving forward....
      {
        forward();
      }
    }
  }
}
