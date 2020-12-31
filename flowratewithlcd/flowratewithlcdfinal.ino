/*
This tutorial Demonstrates the use of Flow Sensor
 Web: blog.circuits4you.com
  The circuit:
Flow Sensor Connections
Yellow --- Pin 6
Red    --- +5V
Black  --- GND     
      
LCD Connections      
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 6
 * LCD D5 pin to digital pin 5
 * LCD D6 pin to digital pin 4
 * LCD D7 pin to digital pin 3
 * LCD R/W pin to ground
 * 1K resistor:
 * One end to ground
 * Another end to LCD VO pin (pin 3)
 
 This example code is in the public domain.
 */

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 6, 5, 4, 3);

volatile int FlowPulse; //measuring the rising edges of the signal
int Calc;                               
int flowsensor = 2;    //The pin location of the sensor


void setup() {
     
   pinMode(flowsensor, INPUT); //initializes digital pin 2 as an input
   Serial.begin(9600);         //This is the setup function where the serial port is initialised,
   attachInterrupt(0, rpm, RISING); //and the interrupt is attached
   
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.setCursor(0, 1);
  lcd.print("Flow Meter");
}

void loop() {
     
 FlowPulse = 0;      //Set NbTops to 0 ready for calculations
 sei();            //Enables interrupts
 delay (1000);      //Wait 1 second
 cli();            //Disable interrupts
 Calc = (FlowPulse * 60 / 7.5); //(Pulse frequency x 60) / 7.5Q, = flow rate in L/hour 
 Serial.print (Calc, DEC); //Prints the number calculated above
 Serial.print (" L/hour\r\n"); //Prints "L/hour" and returns a  new line
  
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 2);
  lcd.print(Calc,DEC);   // print the Flow Rate
  lcd.print(" L/hour");
}

void rpm ()     //This is the function that the interupt calls 
{ 
  FlowPulse++;  //This function measures the rising and falling edge of the hall effect sensors signal
} 
