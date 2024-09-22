/*

THETIS USB Controler VFO
F4IRX - 22/09/2024
Version 0.1a - Beta
Version 20240922 - Beta Version




TODO 
====



DESCRIPTION Fontionnality
=========================

The USB VFO Controler simulate the keyb oard Shorcut for moving VFO frequency
The Clic is present for selecting the digit of Frequency Clic + Left Move the cursor x10 Clic + Right move /10
If the LED Blink then the controler enter in Change Resolution Frequency
Use DEBUG for Serial Information


*/



#include <Bounce2.h>
#include "HID-Project.h"


 //#define DEBUG


#ifdef DEBUG
  #define DEBUG_PRINT(x)  Serial.println (x)
#else
  #define DEBUG_PRINT(x)
#endif

const int FreqUP[] = {4,29,8,21,23,28,24} ; // see qwerty code
const int FreqDW[] = { 20,22,7, 9,10,11,13} ;

const byte ENCODER_PINA= 6;
const byte ENCODER_PINB= 7;
const byte PinSW= 8;
const byte LED= 4;

int valueA; //debounced encoder switch reads
int valueB;
bool valueSens; //pour definir le changement de resolution
int valueFreq=4; //pour la valeur de FreqUP et FreqDW

bool motionDetected = false;
bool CW;
int ledState = LOW;  // ledState used to set the LED
unsigned long previousMillis = 0;  
const long interval = 100;

// Instantiate 2 Bounce object
Bounce debouncerA = Bounce(); 
Bounce debouncerB = Bounce(); 

// setup ********************************************
void setup() {
  Serial.begin(9600);
  Serial.println("Setup");
  pinMode(ENCODER_PINA,INPUT_PULLUP);
  pinMode(ENCODER_PINB,INPUT_PULLUP);
  pinMode(PinSW, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  // After setting up the button, setup debouncer
  debouncerA.attach(ENCODER_PINA);
  debouncerA.interval(2);
  debouncerB.attach(ENCODER_PINB);
  debouncerB.interval(2);
  BootKeyboard.begin();
}

// loop *****************************************
void loop() {
 
  if (!(digitalRead(PinSW))) {
     DEBUG_PRINT("CLIC");
     valueSens=1;
    delay(250);    
  }

 unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval && valueSens==1) {
     previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    digitalWrite(LED, ledState);
  }
  // Update the debouncers
  debouncerA.update();
  debouncerB.update();
  // Read the encoder switches
  valueA = debouncerA.read();
  valueB = debouncerB.read();


  if (valueA && valueB && motionDetected ) //in a detent and just arrived
  {
     motionDetected = false;
   }
  if (valueA && !valueB && !motionDetected ) // just started CW
  {

    CW= true;
    motionDetected=true;
    if (valueSens) {
      valueSens=0;
       digitalWrite(LED, HIGH);
      valueFreq+=1;
      if(valueFreq>6) valueFreq=6;
      DEBUG_PRINT(valueFreq);
    } else {
      DEBUG_PRINT("CW");
      DEBUG_PRINT(FreqUP[valueFreq]);
       BootKeyboard.write(KeyboardKeycode(FreqUP[valueFreq])); 
    }
  }

  if (!valueA && valueB && !motionDetected )  //just started CCW
  {
    CW= false;
    motionDetected=true;
    if (valueSens) {
      valueSens=0;
       digitalWrite(LED, HIGH);
      valueFreq-=1;
      if(valueFreq<0) valueFreq=0;
      DEBUG_PRINT(valueFreq);
    } else {
      DEBUG_PRINT("CCW");
      DEBUG_PRINT(FreqDW[valueFreq]);
      BootKeyboard.write(KeyboardKeycode(FreqDW[valueFreq])); 

    }
  }

} //loop


