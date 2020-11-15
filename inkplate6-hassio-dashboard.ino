/*
   Home Assistant Hub
   An Inkplate 6 personal project to display Home Assistant Data
   using the HA API through HTTP calls
   * HA setup required to work!

   This work was hugely based on the e-Radionica tutorial: Real time Weather station

   For this example you will need only USB cable and Inkplate 6.
   Select "Inkplate 6(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6(ESP32)" option? Follow the tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/


   IMPORTANT:
   WIFI credentials and Home Assistant information must be in the Secrets.h file. Fill and rename "Secrets_example.h"
*/

/*
   Icons made by [Freepik](https://www.flaticon.com/authors/freepik)
   More on [Flaticon](www.flaticon.com)
*/

//---------- CHANGE HERE  -------------:

// City name to de displayed on the top
char city[128] = "Val-d'Epy";


// Delay (seconds) between refreshes. No limit from HA, just think of relevantness and battery life!
#define DELAY_S 10
#define SENSOR_REFRESH_MOD = 1
#define LOCAL_REFRESH_MOD = 5
#define CITIES_REFRESH_MOD = 10

//----------------------------------

#include <Inkplate.h> // Inkplate general library
#include "Network.h"  //Header file for easier code readability
#include "Helpers.h"  // General functions


// Including fonts used
#include "Fonts/Roboto_Light_24.h"
#include "Fonts/Roboto_Light_36.h"
#include "Fonts/Roboto_Light_48.h"
#include "Fonts/Roboto_Light_72.h"
//#include "Fonts/Roboto_Light_120.h"

// Icons in PROGMEM + dedicated functions

// Additional images
#include "Binary_images/Low_battery.h"
#include "Binary_images/Hassio_logo_200px.h"

// Inkplate object
Inkplate display(INKPLATE_1BIT);

// All our network functions are in this object, see Network.h
Network network;



// Variables for local forecasts
char today[3][24] = {
  "99.9",
  "-9.9",
  "exceptional",
};
char tomorr[3][24] = {
  "88.8",
  "-8.8",
  "exceptional",
};

// Variables for storing sensors' data
char sensor1[4][12] = {
  "Cuisine",
  "90.0",
  "9000",
  "90.0",
};
char sensor2[4][12] = {
  "Chambre",
  "80.0",
  "8000",
  "80.0",
};
char sensor3[4][12] = {
  "Grange",
  "70.0",
  "7000",
  "70.0",
};

// Variables for storing cities' data
char city1[4][24] = {
  "Lyon",
  "77.7",
  "-7.7",
  "exceptional",
};
char city2[4][24] = {
  "Suhr",
  "66.6",
  "-6.6",
  "exceptional",
};
char city3[4][24] = {
  "Chambly",
  "55.5",
  "-5.5",
  "exceptional",
};

// Partial Updates require a full refresh once in a while
long refreshes = 0;         // counter
const int fullRefresh = 10; // modulo
int drawLevel;

// Variables for storing current time and weather info
char currentTime[128] = "Not set";
char currentTemp[16] = "99.9";
char currentWind[16] = "99";
char nextSunrise[6] = "0h00";
char nextSunset[6] = "24h00";
char expectedRain[6] = "9.9";
char currentWeather[128] = "Weather not set";
char currentWeatherIcon[20] = "exceptional";





// functions defined below
void drawWeather();
void drawSensors(int drawLevel = 0);
void drawCity();
void drawTime();
void drawCities();





void setup()
{
  // Initializing
  Serial.begin(115200);
  display.begin();
  display.clearDisplay(); // Clear Buffer
  display.setTextWrap(false); // Better without it :)  
  display.setTextColor(BLACK, WHITE);
  display.setTextSize(1);

  // 1bit mode: display.drawBitmap(pos_x, pos_y, progmem_const, width, height, color = BLACK | WHITE, color_bg = WHITE | BLACK);
  display.drawBitmap(50, 212, Hassio_logo_200px, 200, 176, BLACK);
  display.display();
  delay(300);

  // Text
  printText((char *)F("Home Assistant Hub"), 270, 260, &Roboto_Light_48);
  display.partialUpdate();
  delay(500);
  printText((char *)F("> Powered by an Inkplate 6"), 270, 310, &Roboto_Light_36);
  display.partialUpdate();
  delay(500);
  printText((char *)F("> Created by Trytodosth"), 270, 360, &Roboto_Light_36);
  display.partialUpdate();
 
  
  // Initiating Network
  network.begin(); // Before the delay: hidden waiting
  //delay(500); // Wait a bit more before proceeding
  display.clearDisplay(); // Clears buffer
}






void loop()
{
  // Clear display
  //display.clearDisplay();

  // Get Data
  while (!network.getData(currentTime, currentTemp, currentWind, currentWeather, currentWeatherIcon, expectedRain, nextSunrise, nextSunset,
                          today[0], today[1], today[2], tomorr[0], tomorr[1], tomorr[2],
                          sensor1[1], sensor1[2], sensor1[3],
                          sensor2[1], sensor2[2], sensor2[3],
                          sensor3[1], sensor3[2], sensor3[3],
                          city1[1], city1[2], city1[3], city2[1], city2[2], city2[3], city3[1], city3[2], city3[3]))
  {
    Serial.println("Retrying fetching data!");
    delay(5000);
  }

  drawLevel = refreshes % fullRefresh;
  
  //  SENSOR_REFRESH_MOD = 1
  //  LOCAL_REFRESH_MOD = 5
  //  CITIES_REFRESH_MOD = 10
  
  // Draw data, see functions below for info
  drawWeather();
  drawCity();
  drawTime();
  drawCities();
  drawSensors(drawLevel);
  
  // Refresh full screen every fullRefresh times, defined above
  if (drawLevel == 0)
    display.display();
  else
    display.partialUpdate();
  ++refreshes;

  // Go to sleep before checking again
  esp_sleep_enable_timer_wakeup(1000000L * DELAY_S);
  (void)esp_light_sleep_start();
}



////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Function for drawing weather info
void drawWeather()
{
  display.setTextColor(BLACK, WHITE);
  
  // Main icon
  drawWeatherIcon_medium(30, 30, currentWeatherIcon);
  
  // Temperature
  printText(currentTemp, 40, 300, &Roboto_Light_72);
  printCelsiusAtCursor(&Roboto_Light_36, 36);

  // Wind
  drawWindIcon();
  printText(currentWind, 105, 360, &Roboto_Light_36);
  printTextAtCursor((char *)F("m/s"), &Roboto_Light_24);
  
  // Rain
  drawRaindropIcon();
  printText(expectedRain, 105, 410, &Roboto_Light_36);
  printTextAtCursor((char *)F("mm"), &Roboto_Light_24);

  // Sunrise and sunset
  drawSunriseIcon();
  printText(nextSunrise, 335, 360, &Roboto_Light_36);
  drawSunsetIcon();
  printText(nextSunset, 335, 410, &Roboto_Light_36);


  // Forecast Today
  drawWeatherIcon_small(370-120, 70, today[2]);
  printText((char *)F("Ajourd."), 370-20, 70+35, &Roboto_Light_36);
  printText(today[0], 370, 70+70, &Roboto_Light_36);
  printCelsiusAtCursor(&Roboto_Light_24, 24);
  printText(today[1], 370, 70+70+35, &Roboto_Light_36);
  printCelsiusAtCursor(&Roboto_Light_24, 24);
  
  // Forecast Tomorrow
  drawWeatherIcon_small(370-120, 70+125, tomorr[2]);
  printText((char *)F("Demain"), 370-20, 70+35+125, &Roboto_Light_36);
  printText(tomorr[0], 370, 70+70+125, &Roboto_Light_36);
  printCelsiusAtCursor(&Roboto_Light_24, 24);
  printText(tomorr[1], 370, 70+70+35+125, &Roboto_Light_36);
  printCelsiusAtCursor(&Roboto_Light_24, 24);
}



////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Function for drawing current time
void drawTime()
{
  // Drawing current time
  eraseAndPrint(currentTime, 560, 30, &Roboto_Light_24, BLACK, 240);
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Function for drawing city name
void drawCity()
{
  // Drawing city name
  printTextColor(city, 800/3-9*strlen(city), 45, &Roboto_Light_48, BLACK);
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Function for drawing sensors data
void drawSensors(int drawLevel)
{
  // Defining the grid
  int rectWidth = 240;
  int rectThickness = 2;
  int rectSpacing = (800 - rectWidth * 3) / 4;
  int textMargin = 6;
  
  if (drawLevel == 0)
  {
    // Horizontal
    display.fillRect(1 * rectSpacing + 0 * rectWidth, 480, rectWidth - 50, 1, BLACK);
    display.fillRect(2 * rectSpacing + 1 * rectWidth, 480, rectWidth - 50, 1, BLACK);
    display.fillRect(3 * rectSpacing + 2 * rectWidth, 480, rectWidth - 50, 1, BLACK);
  
    // Vertical
    display.fillRect(1 * rectSpacing + 0 * rectWidth, 440, 2, 150, BLACK);
    display.fillRect(2 * rectSpacing + 1 * rectWidth, 440, 2, 150, BLACK);
    display.fillRect(3 * rectSpacing + 2 * rectWidth, 440, 2, 150, BLACK);

    // Sensor names (fixed)
    printText(sensor1[0], 1*rectSpacing+0*rectWidth+textMargin, 440+textMargin+30, &Roboto_Light_36); // Name
    printText(sensor2[0], 2*rectSpacing+1*rectWidth+textMargin, 440+textMargin+30, &Roboto_Light_36); // Name
    printText(sensor3[0], 3*rectSpacing+2*rectWidth+textMargin, 440+textMargin+30, &Roboto_Light_36); // Name
  } else {
    // Fill with white to erase old values
    display.fillRect(1*rectSpacing+0*rectWidth+2, 480+1, rectWidth, 110, WHITE);
    display.fillRect(2*rectSpacing+1*rectWidth+2, 480+1, rectWidth, 110, WHITE);
    display.fillRect(3*rectSpacing+2*rectWidth+2, 480+1, rectWidth, 110, WHITE);
  }

  // Sensor #1
  printText(sensor1[1], 1*rectSpacing+0*rectWidth+textMargin, 440+textMargin+85, &Roboto_Light_48); // Temperature
  printCelsiusAtCursor(&Roboto_Light_24, 24);
  printText(sensor1[2], 1*rectSpacing+0*rectWidth+textMargin, 440+textMargin+135, &Roboto_Light_36);// Pressure
  printTextAtCursor((char *)F("hPa / "), &Roboto_Light_24);
  printTextAtCursor(sensor1[3], &Roboto_Light_36);    // Humidity
  printTextAtCursor((char *)F("%"), &Roboto_Light_24);
  
  // Sensor #2
  printText(sensor2[1], 2*rectSpacing+1*rectWidth+textMargin, 440+textMargin+85, &Roboto_Light_48); // Temperature
  printCelsiusAtCursor(&Roboto_Light_24, 24);
  printText(sensor2[2], 2*rectSpacing+1*rectWidth+textMargin, 440+textMargin+135, &Roboto_Light_36);// Pressure
  printTextAtCursor((char *)F("hPa / "), &Roboto_Light_24);
  printTextAtCursor(sensor2[3], &Roboto_Light_36);    // Humidity
  printTextAtCursor((char *)F("%"), &Roboto_Light_24);

  // Sensor #3
  printText(sensor3[1], 3*rectSpacing+2*rectWidth+textMargin, 440+textMargin+85, &Roboto_Light_48); // Temperature
  printCelsiusAtCursor(&Roboto_Light_24, 24);
  printText(sensor3[2], 3*rectSpacing+2*rectWidth+textMargin, 440+textMargin+135, &Roboto_Light_36);// Pressure
  printTextAtCursor((char *)F("hPa / "), &Roboto_Light_24);
  printTextAtCursor(sensor3[3], &Roboto_Light_36);    // Humidity
  printTextAtCursor((char *)F("%"), &Roboto_Light_24);
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Function for drawing weather info
void drawCities()
{
  int left = 650;
  int space = 125;
  
  // Spacers
  // Vertical
  display.fillRect(500, 80, 2, 310, BLACK);

  // Horizontal
  display.fillRect(left - 50, 175, 100, 1, BLACK);
  display.fillRect(left - 50, 175 + space, 100, 1, BLACK);

  // City 1
  drawWeatherIcon_small(left-120, 55, city1[3]);
  printText(city1[0], left-20, 90, &Roboto_Light_36);
  printText(city1[1], left, 125, &Roboto_Light_36);
  printCelsiusAtCursor(&Roboto_Light_24, 24);
  printText(city1[2], left, 160, &Roboto_Light_36);
  printCelsiusAtCursor(&Roboto_Light_24, 24);
  
  // City 2
  drawWeatherIcon_small(left-120, 55+space, city2[3]);
  printText(city2[0], left-20, 90+space, &Roboto_Light_36);
  printText(city2[1], left, 125+space, &Roboto_Light_36);
  printCelsiusAtCursor(&Roboto_Light_24, 24);
  printText(city2[2], left, 160+space, &Roboto_Light_36);
  printCelsiusAtCursor(&Roboto_Light_24, 24);
  
  // City 3
  drawWeatherIcon_small(left-120, 55+space*2, city3[3]);
  printText(city3[0], left-20, 90+space*2, &Roboto_Light_36);
  printText(city3[1], left, 125+space*2, &Roboto_Light_36);
  printCelsiusAtCursor(&Roboto_Light_24, 24);
  printText(city3[2], left, 160+space*2, &Roboto_Light_36);
  printCelsiusAtCursor(&Roboto_Light_24, 24);
}
