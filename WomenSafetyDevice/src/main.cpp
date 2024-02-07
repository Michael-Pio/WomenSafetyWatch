#include <Arduino.h>

#include "SPI.h"
#include "TFT_eSPI.h"

#include <TouchScreen.h>


#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for most acurate BPM math
#include <PulseSensorPlayground.h>     // Includes the PulseSensorPlayground Library


int Threshold = 550;           // Determine which Signal to "count as a beat" and which to ignore
                               
PulseSensorPlayground pulseSensor;  // Creates an object



#define PulseWire 34

#define MINPRESSURE 10
#define MAXPRESSURE 40000
TFT_eSPI tft = TFT_eSPI();       // Invoke custom library
//const int XP = 6, XM = A2, YP = A1, YM = 7; //ID=0x9341
const int XP = 27, XM = 15, YP = 4, YM = 14; //ID=0x9341

const int TS_LEFT = 380, TS_RT = -2700, TS_TOP = -2500, TS_BOT = 520;

// const int XP=14,XM=4,YP=15,YM=27; //240x320 ID=0x9341
// const int TS_LEFT=882,TS_RT=86,TS_TOP=905,TS_BOT=54;


TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
int pixel_x, pixel_y;     //Touch_getXY() updates global vars
bool Touch_getXY(void)
{
    
  TSPoint p = ts.getPoint();
  p.z=abs(p.z);
  
  if(!(p.z)==0)
  {
    Serial.println(p.z);
    Serial.print("pixel_x:");
    Serial.print(p.x);
    Serial.print(", pixel_y:");
    Serial.println(p.y);
  }
 
  bool pressed = (p.z>0 && p.y < 700);
   
  if (pressed) 
  {
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
          
    pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width()); //.kbv makes sense to me
    pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
  
    Serial.print("pixel_x:");
    Serial.print(pixel_x);
    Serial.print(", pixel_y:");
    Serial.println(pixel_y);
    
    p.z=0;
  }
  return pressed;
}

void setup()
{
  Serial.begin(9600);
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLUE);
  tft.setTextSize(2);
  tft.setCursor(40,20);
  tft.setTextColor(TFT_WHITE);
  tft.print("Smart Device");
  Serial.println("Smart Device");
  tft.setTextSize(2); 

  // Configure the PulseSensor object, by assigning our variables to it
	pulseSensor.analogInput(PulseWire);   
	pulseSensor.blinkOnPulse(LED_BUILTIN);       // Blink on-board LED with heartbeat
	pulseSensor.setThreshold(Threshold);
// Double-check the "pulseSensor" object was created and began seeing a signal
	if (pulseSensor.begin()) {
		Serial.println("PulseSensor object created!");
	}
}

void loop()
{   

  int myBPM = pulseSensor.getBeatsPerMinute();      // Calculates BPM

  tft.setTextSize(4);
  tft.setTextColor(TFT_WHITE);
  tft.fillRect(40,120,300,30,TFT_BLUE);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(40,120);
  tft. print ("BPM: ");

	if (pulseSensor.sawStartOfBeat()) {               // Constantly test to see if a beat happened
		Serial.println("A HeartBeat Happened ! "); // If true, print a message
		Serial.print("BPM: ");
		Serial.println(myBPM);                        // Print the BPM value
    tft.print(myBPM);
    delay(200);
  }
  delay(20);
}
