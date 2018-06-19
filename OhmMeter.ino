// Autorange Ohm Meter T G Bailey June 2018
// using FET's to switch range resistors
// uses Ohms Law to calculate unknown resistor value
// First takes a reading across test resistor will be a value between 0 - 1023
// Multiply this by volt/bit (reading * 5/1024) == volts across resistor
// Subtract this from 5V (voltage across the test circuit) 
// This value is the voltage across the fixed resistor who's value we know
// simply use Ohms Law to determine the circuit current I = V/R
// As same current is flowing through the unknown resistor, use Ohms Law again
// To calculate the unknown resistor value using Voltage obtained from the initial reading, R = V/I
// To autorange, takes a reading, checks if the value of the reading is 1/2 the voltage or less
// If so use this range, if no change to next higher range and try again until Max range is selected
#define DEBUG 0 // flag to turn on/off debugging
#define Serial if(DEBUG)Serial 

#include <LiquidCrystal_I2C.h>

#define Range100 2 // Digital pins for ranges
#define Range1K 3
#define Range10K 4
#define Range100K 5
#define Range1M 6
#define Range10M 7

int RangePins[] = {2,3,4,5,6,7}; // Digital pins used for ranges
float SeriesR[] = {342.0,3900.0,33000.0,390000.0,982000.0,10000000.0}; // exact resistance values used
char Range[][8] = {" Ohms"," K-Ohms"," M-Ohms"};  // for display purposes
int analogPin = 0;    // takes all readings
int Reading = 0;      // A0 reading
float Vin = 5.0;      // Voltage across resistors
double Current = 0.0; // For current calculation
float R1 = 0.0;       // Fixed resistor for measurement, changes by selecting ranges
float R2 = 0.0;       // Resistor under test
float Voltage = 0.0;  // For voltage across test resistor
float BitVal = Vin/1024.0;  // Voltage per bit
float VR2 = 0.0;
float VR1 = 0.0;
int i;
int CurrRange;
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  for(i = 2;i < 8;i++) // set mode for range pins
  {
    pinMode(i,OUTPUT);
    digitalWrite(i,LOW); // turn all off
  }
  Serial.begin(115200);
  analogReference(DEFAULT); // 5V
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  lcd.clear();
  
  CurrRange=Range10M; // set some defaults
  R1 = SeriesR[5];
}

void loop()
{
  
  setRange();
 
  Reading = analogRead(analogPin);
  Serial.print(Reading);
  Serial.print(" ");
  if(Reading) 
  {
    VR2 = Reading * BitVal;   // Voltage at junction
    VR1 = (Vin - VR2); //R1;  // Voltage across R1
    Current = VR1/R1;         // Current through R1
    R2 = VR2/Current;         // Same current through R2, so use Ohms law R = V/I
    Serial.print("Range = ");
    Serial.print(CurrRange);
    Serial.print(" VR2 = ");
    Serial.println(VR2);
    lcd.clear();
    lcd.setCursor(0,0);
    if(CurrRange == Range10M && VR2 > 1.1)
    {
      Serial.println("Out of Limits...");
      lcd.setCursor(0,0);
      lcd.print("**************");
      lcd.setCursor(0,1);
      lcd.print("Out of Limits");
      delay(1000);
    }
    else
    {
      if(R2 < 1000)
      {
        lcd.setCursor(0,0);
        lcd.print("R = ");
        lcd.print(R2);
        lcd.setCursor(0,1);
        lcd.print("   ");
        lcd.print(Range[0]);
        lcd.print("   ");
        Serial.print("R2:= ");
        Serial.print(R2);
        Serial.println(Range[0]);
      }
      else if(R2 > 1000 && R2 < 1000000)
      {
        lcd.setCursor(0,0);
        lcd.print("R = ");
        lcd.print(R2/1000.0);
        lcd.setCursor(0,1);
        lcd.print("   ");
        lcd.print(Range[1]);
        lcd.print("   ");
        Serial.print("R2:= ");
        Serial.print(R2/1000.0);
        Serial.println(Range[1]);
      }
      else
      {
        lcd.setCursor(0,0);
        lcd.print("R = ");
        lcd.print(R2/1000000.0);
        lcd.setCursor(0,1);
        lcd.print("   ");
        lcd.print(Range[2]);
        lcd.print("   ");
        Serial.println(Range[2]);
        Serial.print("R2:= ");
        Serial.print(R2/1000000.0);
      }
    delay(1000);
    }
  }
  else
  {
    lcd.setCursor(0,0);
    lcd.print("R = ");
    lcd.print(0);
    lcd.print("   ");
    lcd.print(Range[0]);
    lcd.print("   ");
    lcd.setCursor(0,1); 
    lcd.print("                ");   
    Serial.println("0 Ohms");
    delay(1000);
  }
}

void setRange()
{
    int Reading;
    for(i=0;i<6;i++)  // Cycle through ranges
    {
      digitalWrite(CurrRange,LOW);      // turn current FET off
      CurrRange = RangePins[i];         // remember which FET is active
      R1 = SeriesR[i];                  // fixed resistor for calculation
      digitalWrite(CurrRange, HIGH);    // turn appropriate FET on
      delay(10);                        // short delay (settling time)
      Reading = analogRead(analogPin);  // take a reading
      if(Reading < 512)                 // correct range set
        return;
    }
      
}
