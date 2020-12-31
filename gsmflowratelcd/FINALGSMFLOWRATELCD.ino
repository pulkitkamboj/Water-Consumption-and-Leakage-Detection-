#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
LiquidCrystal lcd(12, 11, 6, 5, 4, 3);
SoftwareSerial mySerial(9, 10);
byte statusLed    = 13;
const int pwm = 2 ; //initializing pin 2 as pwm
const int in_1 = 7 ;
const int in_2 = 8 ;
//For providing logic to L298 IC to choose the direction of the DC motor

byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin       = 2;
// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float calibrationFactor = 4.5;

volatile byte pulseCount;  

float flowRate;
float oldflowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

unsigned long oldTime;
int count1;
void setup()
{
  
  // Initialize a serial connection for reporting values to the host
  Serial.begin(9600);
  mySerial.begin(9600);   // Setting the baud rate of GSM Module
  delay(200);
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(200);  // Delay of 200 milli seconds
  lcd.begin(16, 2);
  // Set up the status LED line as an output
  pinMode(statusLed, OUTPUT);
  digitalWrite(statusLed, HIGH);  // We have an active-low LED attached
  
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;
  oldflowRate       = 0.0;
  count1            = 0;

  // The Hall-effect sensor is connected to pin 2 which uses interrupt 0.
  // Configured to trigger on a FALLING state change (transition from HIGH
  // state to LOW state)
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
}
void loop()
{  
  if((millis() - oldTime) > 1000)    // Only process counters once per second
  { 
    // Disable the interrupt while calculating flow rate and sending the value to
    // the host
    detachInterrupt(sensorInterrupt);
    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    oldflowRate=flowRate;
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    if(oldflowRate-flowRate<100 && flowRate!=0) //later on we will change so that if there is small change in flow rate this part will still run
    {
      count1++; //increments if flow rate is constant
    }
    else
    {
      count1=0;
    }
    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    oldTime = millis();
    
    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;
    
    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;
    unsigned int frac;
    
    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable
    Serial.print(".");             // Print the decimal point
    // Determine the fractional part. The 10 multiplier gives us 1 decimal place.
    frac = (flowRate - int(flowRate)) * 10;
    Serial.print(frac, DEC) ;      // Print the fractional part of the variable
    Serial.print("L/min");
    // Print the number of litres flowed in this second
    Serial.print("  Current Liquid Flowing: ");             // Output separator
    Serial.print(flowMilliLitres);
    Serial.print("mL/Sec");

    // Print the cumulative total of litres flowed since starting
    Serial.print("  Output Liquid Quantity: ");             // Output separator
    Serial.print(totalMilliLitres);
    Serial.println("mL");
    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print("Rate: ");
    lcd.print(int(flowRate));   // print the Flow Rate
    lcd.print(".");
    lcd.print(frac,DEC);
    lcd.print(" L/hour");
    lcd.setCursor(0,1);
    lcd.print("Total: ");
    lcd.print(totalMilliLitres);
    lcd.print(" mL");

    // Reset the pulse counter so we can start incrementing again
    pulseCount = 0;
    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
    if(count1==5)//if water runs for ten seconds it will send an message to the mobile phone
    {
      Serial.print("deguwieweuhw");
      SendMessage();
    }
  }
}

/*
Insterrupt Service Routine
 */
void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}
void SendMessage()
{
  mySerial.println("AT+CMGS=\"+918577050782\"\r");
  delay(4000);
  mySerial.println("Your tap is running for a long time, we are turning off the supply");// The SMS text you want to send
  delay(4000);
  mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}
