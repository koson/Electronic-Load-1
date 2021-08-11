#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Function Prototypes
void drawBarGraph(int level); //Function prototype to draw a bar graph at the top of the screen

//Defines for OLED screen
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32


//Variables
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //Create the oled screen variable

int barGraphLevel = 0; //Holds the level for the bar graph
int currentReading = 0; //Holds the current measured by the ADC
int currentSet = 0; //Holds the setting the load will try and maintain


void setup() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
}

void loop() {
  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text

  drawBarGraph(90);
  display.setCursor(0, 8);     // Start at top-left corner
  display.write("Current Set: 1.250");
  display.setCursor(0, 16);     // Start at top-left corner
  display.setTextSize(2);      // Normal 2:1 pixel scale
  display.write("1.360 A");
  display.display();

  delay(2000);

}

void drawBarGraph(int level){ //level is between 0 and 100%
    display.setCursor(0, 0);     // Start at top-left corner
    for(int i = 0; i < map(level,0,100,0,128); i++){
      display.drawFastVLine(i,0,8,SSD1306_WHITE);
    }
}
