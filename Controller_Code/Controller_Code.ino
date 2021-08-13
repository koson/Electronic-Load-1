#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Function Prototypes
void drawBarGraph(int level); //Function prototype to draw a bar graph at the top of the screen
double readTemp(); //Function prtototype for reading a converting the NTC voltage to a temperature

//Defines for OLED screen
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define fanPin = 8 //Defines which pin the fan control is on
#define errorPin = 10 //defines the pin the error LED is on
#define tempPin =  A7 //defines which pin the temp sensor is read on
#define feedbackPin = A0 //defines which pin the feedback voltage is read on




//Variables
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); //Create the oled screen variable

int barGraphLevel = 0; //Holds the level for the bar graph
int currentReading = 0; //Holds the current measured by the ADC
int currentSet = 0; //Holds the setting the load will try and maintain

double tempcoeff[5] = {-511.9587, 435.7398, -153.1109, 29.91921,-1.633023}; //defines the 4th order polynomial the temperature is read from


void setup() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
}

void loop() {
  
  if(readTemp() > fanOnTemp){ //Read the temperature and if it is over the set limit, turn the fan on at full speed
    digitalWrite(fanPin, HIGH);
  }else{
    digitalWrite(fanPin, LOW);
  }

  currentReading = map(analogRead(feedbackPin)*(5.0/1023.0)*0.0267; //Read the analog voltage and convert it to a current reading via the current sensor

  display.clearDisplay(); //Clear the display
  
  drawBarGraph(currentSet/65.0*100.0); //Convert the setting to a percentage to draw on the bar graph

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 8);     // Start on second line
  String set = "Current Set: " + (String)(currentSet);
  display.write(set.c_str(), set.length());
  
  display.setCursor(0, 16);     // Start on the third line
  display.setTextSize(2);      // Normal 2:1 pixel scale
  String reading = (String)(currentReading);
  display.write(reading.c_str(),reading.length());
  display.display();

  delay(2000);

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
