#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Function Prototypes
void drawBarGraph(int level); //Function prototype to draw a bar graph at the top of the screen
double readTemp(); //Function prtototype for reading a converting the NTC voltage to a temperature
void setDisplay(); //Function prototype for setting the variables on the display
void changeCurrent(); //Function prototype to run when the switch is pressed

//Defines for OLED screen
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels (This is a 64 line display even though the address would say it is the 32 line display) (Text is displayed at 8 pixels per line for a scaling of 1)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define fanPin 9 //Defines which pin the fan control is on
#define errorPin 10 //defines the pin the error LED is on
#define tempPin  A7 //defines which pin the temp sensor is read on
#define feedbackPin A0 //defines which pin the feedback voltage is read on
#define switchPin 8 //define which pin the switch to change the set current lives on

#define fanOnTemp 50 //define the temperature at which the fan turns on
#define thermalLimit 100 //defines the temperature at which the MOSFETS will turn off to avoid damage
#define resetTemp 30 //defines what temperature the system must come to after an overtemp to start back up

#define timeout 2000 //defines how long the system waits for changes to stop for


//Variables
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); //Create the oled screen variable

int barGraphLevel = 0; //Holds the level for the bar graph
int currentReading = 0; //Holds the current measured by the ADC
int currentSet = 0; //Holds the setting the load will try and maintain
bool currentChanging = 0; //holds whether the current level will be changing, changes when the switch is pressed and after a short timeout with no changes from the rotary encoder

double tempcoeff[5] = {-511.9587, 435.7398, -153.1109, 29.91921,-1.633023}; //defines the 4th order polynomial the temperature is read from

unsigned long lastChangeTime = 0;

void setup() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    digitalWrite(errorPin, HIGH); //Turn the error LED on
    for(;;); // Don't proceed, loop forever
  }

  attachInterrupt(digitalPinToInterrupt(switchPin), changeCurrent, FALLING); //Have the switch able to interrupt the system to set the current level
}

void loop() {
  if(readTemp() > thermalLimit){ //Read the temperature and if it is over the set limit, turn the fan on at full speed
    currentSet = 0;
    while(readTemp() > resetTemp){ //Blink the error LED until the system cools off enough to restart
      digitalWrite(errorPin, HIGH);
      delay(500);
      digitalWrite(errorPin, LOW);
      delay(500);
    }
  }else if(readTemp() > fanOnTemp){ //If the system is starting to get hot, turn the fan on
    digitalWrite(fanPin, HIGH);
  }else{ //otherwise, not enough power is flowing to need the fan
    digitalWrite(fanPin, LOW);
  }

  while(currentChanging == 1 && millis()-lastChangeTime < timeout){ //If the current needs to change, then wait for the changes to finish without allowing anything else to change
    //put code to run when the current needs to change
  }

  currentReading = map(analogRead(feedbackPin)*(5.0/1023.0)*0.0267; //Read the analog voltage and convert it to a current reading via the current sensor

  setDisplay(); //Set the variables on the screen

  delay(500);
}

void drawBarGraph(int level){ //level is between 0 and 100%
    display.setCursor(0, 0);     // Start at top-left corner
    for(int i = 0; i < map(level,0,100,0,128); i++){
      display.drawFastVLine(i,0,8,SSD1306_WHITE);
    }
}

double readTemp(){
  double voltage = map(analogRead(tempPin),0,1023,0,5); //Read the value from the analog pin and convert it to a voltage value
  double temp = 0;
  for(int i = 0; i < 5; i++){ //loop through all the coeeficients and calculate the temperature
    temp += tempcoeff[i]*voltage^i; //Sum each polynomial
  }
  return temp; //Return the temperature reading
}

void setDisplay(){
  display.clearDisplay(); //Clear the display
  
  drawBarGraph(currentSet/65.0*100.0); //Convert the setting to a percentage to draw on the bar graph
  
  display.setCursor(0, 8);     // //Put cursor at the current set line
  display.setTextSize(1);      // Normal 1:1 pixel scale
  String set = "Current Set: " + (String)(currentSet);
  display.write(set.c_str(), set.length());
  
  display.setCursor(0, 16);     // Put cursor at the actual reading line
  display.setTextSize(2);      // Normal 2:1 pixel scale
  String reading = (String)(currentReading);
  display.write(reading.c_str(),reading.length());
  display.display();
}

void changeCurrent(){
  currentChanging = 1; //set the status to allow changing the current
}
