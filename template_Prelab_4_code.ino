// /*
//  * Lab 4
//  *
//  * Created (Date): 27 Mar 2024
//  * Author (Name, netID): Akinfolami Akin-Alamu, aoa9
//  */

// // --- Global Variables ---

// // "period" : stores the value of the output wave period (not necessarily in seconds)
// volatile float period;

// // "timer" : stores the value of TIMER1
// volatile unsigned int timer1Value = 0;

// // --- ISR: an interrupt function that resets and reads the timer value

// ISR(PCINT0_vect)
// {
//   // resets the timer to zero on a rising edge,
//   if (PINB & 0b00000100)
//   {
//     TCNT1 = 0;
//   }
//   else
//   {
//     timer1Value = TCNT1;
//   }

//   // and stores the timer value in a variable ("timer") on a falling edge (or vice versa).
// }

// // --- initColor: initializes the interrupt and timer

// void initColor()
// {

//   // a.	Initialize your I/O pins
//   // DDRD = 0b11000100; // Set P6 and P7 as outputs and P2 as output
//   DDRB = 0b00000000; // set pins11 and pins12 as outputs
//   DDRD = 0b01000000;

//   // b. Initialize my pin change interrupt
  

//   // c.	Initialize your timer
//   TCCR1A = 0b00000000; // Normal mode
//   TCCR1B = 0b00000001; // 1024 pre-scaler
// }

// // --- getColor: calculates the period of the sensor output wave and returns it as a variable

// int getColor()
// {

//   // a.	Enable the specific bit for your Pin Change Interrupt
//   PCICR = 0b00000001; // Enable PCIE0
//   PCMSK0 = 0b00000100; // Enable PCINT2

//   // b.	Add a short delay (5 ~ 10 milliseconds)
//   _delay_ms(10);

//   // c.	Disable the specific bit for your pin change interrupt
//   // (to prevent further interrupts until you call getColor again)
//   PCICR = 0b00000000; // Enable PCIE0
//   PCMSK0 = 0b00000000; // Disable PCINT2

//   // d.	Return the period in units of time
//   period = timer1Value * 0.0625 *2;
  
//  // return period;

// }

// int main(void)
// {

//   // call initColor function
//   Serial.begin(9600);
//   initColor();
//   sei();
  

//   while (1)
//   {

//     // call getColor function
//     getColor();
//     if (period > 200){
//       PIND = 0b01000000;
//     }else{
//       PIND = 0b00000000;
//     }


//     // print the "period"
//     Serial.println(period);
//     // Serial.println(timer1Value);

//     // Add a short delay (less than a second)
//     _delay_ms(100);
//   }
// }