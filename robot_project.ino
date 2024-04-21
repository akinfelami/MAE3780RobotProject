
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

char homeColor[] = "";

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

void initColor()
{
  sei();

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
}

int main(void)
{

  // Initialize color sensor
  DDRB = 0b00000000; // ALL pins on B as inputs.
  Serial.begin(9600);
  initColor();
  _delay_ms(10);
  while (1)
  {
    getColor();
    Serial.println(colorSensorPeriod);

    // if (MIN_BLUE_PERIOD <= colorSensorPeriod && colorSensorPeriod <= MAX_BLUE_PERIOD)
    // {
    //   homeColor = "blue";
    // }
    // else if (MIN_YELLOW_PERIOD <= colorSensorPeriod && colorSensorPeriod <= MAX_YELLOW_PERIOD)
    // {
    //   homeColor = "yellow";
    // }

    // less than 150 = yellow
    // greater than 200 = blue

    bool edge_left = PINB & PIN_QTI_LEFT;
    bool edge_right = PINB & PIN_QTI_RIGHT;
    Serial.print("edge_left:");
    Serial.print(edge_left);
    Serial.print(", edge_right:");
    Serial.print(edge_right);
    Serial.println();

    // Color sensor detection logic (Milestone 3):
    // 1. start by noting home color
    // 2. always move forward (keeping edge detection logic in mind)
    // 3. if not at home color, then turn 180 degrees (clockwise for consistency)
    // 4. drive back to home color, once detected home color (drive forward a tiny bit)
    // 5. stop

    // Movement logic for when QTI detects edges (might need to pause a bit between detections)
    // if edge_left and edge_right are both true, (back up a bit and) then turn right (90 degrees)
    // if edge_left is true and edge_right is false, then turn right (90 degrees)
    // if edge_left is true and edge_right is false, then turn left (90 degrees)

    // _delay_ms(10);
  }
}
