// Farhan Ali
// mfa0092
// Due March 26th, 2020
// Lab 3

// Inludes
#include <Wire.h>
#include "Adafruit_TMP006.h"
#define USE_USCI_B1 
Adafruit_TMP006 tmp006;

// Pin Map
const int LEDR = 39;            // Red LED pin
const int LEDG = 38;            // Green LED pin
const int B_ONE = 33;           // pushbutton one
const int B_TWO = 32;           // pushbutton two
const int xpin = 23;            // x-axis of the accelerometer
const int ypin = 24;            // y-axis
const int zpin = 25;            // z-axis (only on 3-axis models)

// Variables
int ledStateRed = LOW;          // Red LED state
int ledStateGreen = LOW;        // Green LED state
int blink = 0;                  // number of blinks
unsigned int cnt1 = 0;          // Counter for no delay check
int Serial_n = 0;               // n command to read second command
String Serial_series ;          // first command
String Serial_led ;             // color command third

// Flags
int ACC = 0;                    // Accelerometer flag
int TEMP = 0;                   // Temperatture flag
unsigned int ENABLE = 0;        // Toggle flag


// Time based
unsigned long previousMillis = 0;
unsigned long currentMillis; 
long interval = 500;             // length of led toggled interval in ms

// Series Function 1, returns int # of blinks
int series1( int &n)             
{
  int series;
  if(n == 1 || n == 2)
  {
   series = 1; 
  }
  else
  {
    int a1 = 1;
    int a2 = 1;
    int a = 0;
    for (int i = 2; i < n; i++)
    {
        a = 2*a1+a2;
        a2 = a1;
        a1 = a;
    }
        
   series = a;
  }
 return series;
}

// Series Function 2, returns int # of blinks
int series2( int &n)   
{
    int series;
  if(n == 1 || n == 2)
  {
   series = 1; 
  }
  else
  {
    int a1 = 1;
    int a2 = 1;
    int a = 0;
    for (int i = 2; i < n; i++)
    {
        a  = a1+a2;
        a2 = a1;
        a1 = a;
    }
        
   series = a;
  }
 return series;
}

// Blink function, void return
void blinkLed(const int &ledPin,int &ledState) 
{
              // if the LED is off turn it on and vice-versa:
              if (ledState == LOW)
              {
                ledState = HIGH;
              }
              else 
              {
                ledState = LOW;
              }
              
              // set the LED with the ledState of the variable:
              digitalWrite(ledPin, ledState);
 }

 // Temperature Interrupt Routine
void ISR_tempr(){TEMP = 1;}

 // Acceleraometer Interrupt Routine
void ISR_accel(){ACC = 1;}

// Setup function
void setup() 
{
 Serial.begin(9600);  
 
  // initialize the LED pins as  output:
  pinMode(LEDR, OUTPUT);  
  pinMode(LEDG, OUTPUT);     
  
  // initialize the pushbutton pin as an input:
  pinMode(B_ONE, INPUT_PULLUP);     
  pinMode(B_TWO, INPUT_PULLUP); 
  
  analogReadResolution(12); // reads at 12 bit resolution
  
  // Interrupt attachments
  attachInterrupt(B_TWO, ISR_accel, LOW);
  attachInterrupt(B_ONE, ISR_tempr, LOW);
  if (! tmp006.begin(TMP006_CFG_8SAMPLE)) // error control
    {
      Serial.println("No sensor found");
      while (1);
    }
}

// MAIN
void loop()  
{
    
    if(Serial.available() > 0 && ENABLE == 0 ) //input read, locks out when enable = 1
    {
      Serial_series = Serial.readStringUntil(' '); // first command
      Serial_n = Serial.parseInt();                // second command
      Serial_led = Serial.readStringUntil('\r');   // third command // corrected \n to \r because of 7.5 pt deduction 
      Serial_led.trim(); // removes spaces
      ENABLE = 1; // enable toggle
    }   
    
    // Case 1 "SR1"
      if((Serial_series == "SR1") && (Serial_n > 0) && (Serial_led == "R") && (ENABLE == 1)) 
      {     
          if(cnt1==0){blink = series1(Serial_n);} // catches blink max value on first cycle
            currentMillis = millis();
            
            if(currentMillis - previousMillis > interval) // How long led should be on or off
            {
                blinkLed(LEDR,ledStateRed);
                previousMillis = currentMillis;
                 if(ledStateRed == LOW){cnt1++;}        // cnt1 counts the cycles or in this case blinks
                 if(cnt1 >= blink){ENABLE = 0; cnt1=0; Serial.println("Executed");}// prints only on last cycle of instruction
            } // closes if Millis()
      } // closes if
      
     // Case 2 "SR2"
      else if((Serial_series == "SR2") && (Serial_n > 0) && (Serial_led == "G") && (ENABLE == 1)) 
      {
        
        if(cnt1==0){blink = series2(Serial_n);}
      
            currentMillis = millis();
            if(currentMillis - previousMillis > interval) 
            {
                blinkLed(LEDG,ledStateGreen);
                 previousMillis = currentMillis;
                 if(ledStateGreen == LOW){cnt1++;}
                 if(cnt1 >= blink){ENABLE = 0; cnt1=0;Serial.println("Executed");}
            } // closes if Millis()
           
      }// closes else if
      
     // Case 3 "Invalid Command"
      else 
      {
        if(ENABLE == 1){ Serial.println("Invalid Command"); ENABLE = 0;}
      }
   
   // Temperature Interrupt Function
     if(TEMP == 1){
       
    float object = tmp006.readObjTempC();
    Serial.print("Temperature in Celsius: ");
    Serial.print(object);
    Serial.println("*C");
    Serial.println("Executed");
    TEMP = 0;
     }
     
   // Accelerometer Interrupt Function
     if(ACC == 1){
      // print the sensor values:
      Serial.print(((int) analogRead(xpin)) - 2048);
      // print a tab between values:
      Serial.print("\t");
      Serial.print(((int) analogRead(ypin)) - 2048);
      // print a tab between values:
      Serial.print("\t");
      Serial.print(((int) analogRead(zpin)) - 2048);
      Serial.println();
      Serial.println("Executed");
      ACC = 0;
      }
}// closes MAIN loop()
