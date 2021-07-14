/*
   Home Assistant Hub
   An Inkplate 6 personal project to display Home Assistant Data
   using the HA API through HTTP calls
     HA setup required to work!

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



//----------------------------------

#include <Inkplate.h> // Inkplate general library
#include "Network.h"  //Header file for easier code readability
#include "Helpers.h"  // General functions
#include "Fields_definitions.h"  // Some definitions (blank data) moved there

// Including fonts used
#include "Fonts/Roboto_Light_24.h"
#include "Fonts/Roboto_Light_36.h"
#include "Fonts/Roboto_Light_48.h"
#include "Fonts/Roboto_Light_72.h"
//#include "Fonts/Roboto_Light_120.h"

// Additional images
#include "Binary_images/Low_battery.h"
#include "Binary_images/Home_screen2.h"


// Class instances
Inkplate display(INKPLATE_1BIT); // Inkplate object
Network network; // Network object



// ------- REFRESH -------
// Delay (seconds) between refreshes. No limit from HA, just think of relevantness and battery life!
const int DELAY_S = 10;
const int SENSOR_REFRESH_MOD = 1;
const int LOCAL_REFRESH_MOD = 5;
const int CITIES_REFRESH_MOD = 10;
//const long SUN_REFRESH_MOD = (long) (14400 / DELAY_S); // Every 4 hours
const long SUN_REFRESH_MOD = 1;

// Partial Updates require a full refresh once in a while
long refreshes = 0;         // counter
const int fullRefresh = 10; // modulo
int drawLevel = 1; // Values only


// ------- BATTERY --------
float voltage;
float voltage_percent;

// Ajustment depending on ESP32 ADC + battery used
float voltage_min = 3.0;
float voltage_max = 4.3;

char voltage_string[10];


// ------- DECLARATIONS AND DEFINITIONS --------
// Variables for local forecasts
extern char today[3][24];
extern char tomorr[3][24];

// Variables for storing sensors' data
extern char sensor1[4][16];
extern char sensor2[4][16];
extern char sensor3[4][16];

// Variables for storing cities' data
extern char city1[4][24];
extern char city2[4][24];
extern char city3[4][24];

// Additional definitions kept here because single lines
char city[128] = "Val-d'Epy";
char currentTime[128] = "Not set";
char currentTemp[16] = "99.9";
char currentWind[16] = "99";
char nextSunrise[6] = "0h00";
char nextSunset[6] = "24h00";
char expectedRain[6] = "9.9";
char currentWeather[128] = "Weather not set";
char currentWeatherIcon[20] = "exceptional";



// Prototypes of functions defined below
void drawCityName();
void drawTime();
void drawBattery();
void drawWeather(int drawLevel = 1);
void drawSensors(int drawLevel = 1);
void drawCities(int drawLevel = 1);
void drawSun(int drawLevel = 1);




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
  display.drawBitmap(0, 0, Home_screen2, 800, 600, WHITE, BLACK);
  printText((char *)F("Bienvenue !"), 270, 100, &Roboto_Light_48);
  display.display();
  delay(1000);

  // Text
  display.setTextColor(WHITE);
  display.setFont(&Roboto_Light_36);
  display.setCursor(20, 580);
  display.print(F("Recherche WiFi..."));
  display.setTextColor(BLACK);
  display.partialUpdate();


  // Initiating Network
  network.begin(); // Before the delay: hidden waiting
  //delay(500); // Wait a bit more before proceeding

  // Preparing the dashboard with fix information (background)
  display.clearDisplay(); // Clears buffer
  drawCityName();
  drawTime();
  drawWeather(0);
  drawNextRain(0);
  drawSensors(0);
  drawCities(0);
  drawSun(0);
  drawLevel = 2; // Only the values
}



void loop()
{
  bool success = false;
  while (!success)
  {
    success = network.getTimestamp(currentTime);// Time always updated
    if (refreshes % SENSOR_REFRESH_MOD == 0) { // Updating sensors
      success = success && network.getSensorsData(sensor1[1], sensor1[2], sensor1[3],
                sensor2[1], sensor2[2], sensor2[3],
                sensor3[1], sensor3[2], sensor3[3]);
      drawSensors(drawLevel);
      success = success && network.getNextRainData(rainNextHour[0], rainNextHour[1], rainNextHour[2],
                rainNextHour[3], rainNextHour[4], rainNextHour[5],
                rainNextHour[6], rainNextHour[7], rainNextHour[8]);
      drawNextRain(drawLevel);
    }
    if (refreshes % LOCAL_REFRESH_MOD == 0) { // Updating local weather info
      success = success && network.getLocalWeatherData(currentTemp, currentWind, currentWeather, currentWeatherIcon, expectedRain, today[0], today[1], today[2], tomorr[0], tomorr[1], tomorr[2]);
      drawWeather(drawLevel);
    }
    if (refreshes % CITIES_REFRESH_MOD == 0) { // Updating other cities weather
      success = success && network.getOtherCitiesData(city1[1], city1[2], city1[3], city2[1], city2[2], city2[3], city3[1], city3[2], city3[3]);
      drawCities(drawLevel);
    }
    if (refreshes % SUN_REFRESH_MOD == 0) { // Updating sunrise and sunset
      success = success && network.getSunData(nextSunrise, nextSunset);
      drawSun(drawLevel);
    }
    drawCityName(); // On top of main icon
    drawBattery();
    drawTime(); // On top of everything else

    if (!success) // Skipped if everyhting OK. Not selective, I know
    {
      Serial.println("Retrying fetching data!");
      delay(5000);
    }
  }
  drawLevel = 2; // Values only. DrawLevel can be edited in Network to redraw all after displaying an error.

  // Refresh full screen every fullRefresh times, defined above
  if (refreshes % fullRefresh == 0)
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
// Function for drawing city name
void drawCityName()
{
  // Drawing city name
  printTextColor(city, 800 / 3 - 9 * strlen(city), 45, &Roboto_Light_48, BLACK);
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Function for drawing current time
void drawTime()
{
  // Drawing current time
  eraseAndPrint(currentTime, 675, 40, &Roboto_Light_36, BLACK, 150);
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Function for drawing current time
void drawBattery()
{
  //temperature = display.readTemperature(); // Read temperature from on-board temperature sensor
  voltage = display.readBattery(); // Read battery voltage (NOTE: Doe to ESP32 ADC accuracy, you should calibrate the ADC!)

  voltage_percent = min((float)100, max((float)0, 100 * (voltage - voltage_min) / (voltage_max - voltage_min)));

  dtostrf(voltage_percent, 3, 0, voltage_string);
  strcat (voltage_string, "% ");

  //  // Current battery voltage
  //  Serial.print(F("Battery level: "));
  //  Serial.print(voltage_string);
  //  Serial.print(F(" = "));
  //  Serial.println(voltage_percent);

  display.fillRect(597, 0, 100, 38, WHITE);
  printText(voltage_string, 597, 34, &Roboto_Light_24);
  drawBatteryIcon(voltage_percent, false, 560, 15);
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Function for drawing weather info
void drawWeather(int drawLevel)
{
  // drawLevel: 0: Background only
  // drawLevel: 1: Background + values
  // drawlevel: 2: Values refresh

  // Local weather
  display.setTextColor(BLACK, WHITE);
  if (drawLevel <= 1)
  {
    // Draw background
    drawWindIcon();
    drawRaindropIcon();
    printText((char *)F("Ajourd."), 370 - 20, 70 + 35, &Roboto_Light_36);
    printText((char *)F("Demain"), 370 - 20, 70 + 35 + 125, &Roboto_Light_36);
  }
  else if (drawLevel >= 1)
  {
    // Erase old values
    display.fillRect(105, 320, 160, 100, WHITE); // Wind and rain
    display.fillRect(370, 105, 75, 100, WHITE); // Forecast today
    display.fillRect(370, 105 + 125, 75, 100, WHITE); // Forecast tomorrow

    // Main icon
    drawWeatherIcon_medium(30, 30, currentWeatherIcon);

    // Temperature
    eraseAndPrint(currentTemp, 40, 300, &Roboto_Light_72, BLACK, 180, 80);
    printCelsiusAtCursor(&Roboto_Light_36, 36);

    // Wind
    printText(currentWind, 105, 360, &Roboto_Light_36);
    printTextAtCursor((char *)F("km/h"), &Roboto_Light_24);

    // Rain
    printText(expectedRain, 105, 410, &Roboto_Light_36);
    printTextAtCursor((char *)F("mm"), &Roboto_Light_24);


    // Forecast Today
    drawWeatherIcon_small(370 - 120, 70, today[2]);
    printText(today[0], 370, 70 + 70, &Roboto_Light_36);
    printCelsiusAtCursor(&Roboto_Light_24, 24);
    printText(today[1], 370, 70 + 70 + 35, &Roboto_Light_36);
    printCelsiusAtCursor(&Roboto_Light_24, 24);
    printText((char *)F("Ajourd."), 370 - 20, 70 + 35, &Roboto_Light_36);

    // Forecast Tomorrow
    drawWeatherIcon_small(370 - 120, 70 + 125, tomorr[2]);
    printText(tomorr[0], 370, 70 + 70 + 125, &Roboto_Light_36);
    printCelsiusAtCursor(&Roboto_Light_24, 24);
    printText(tomorr[1], 370, 70 + 70 + 35 + 125, &Roboto_Light_36);
    printCelsiusAtCursor(&Roboto_Light_24, 24);
    eraseAndPrint(nextSunrise, 335, 360, &Roboto_Light_36, BLACK);
    eraseAndPrint(nextSunset, 335, 410, &Roboto_Light_36, BLACK);
  }
}


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Function for drawing Sun info
void drawNextRain(int drawLevel)
{
  int height_top = 450;
  int space_between = 4;
  int width_5min = 20;
  int height_unit = 30;
  int height_min = 2;
  int left = 40;

  char cstr[8];
  // drawLevel: 0: Background only
  // drawLevel: 1: Background + values
  // drawlevel: 2: Values refresh


  if (drawLevel <= 1)
  {
    // Draw background
    printText((char *)F("Pluie dans l'heure :"), left+50, height_top+14, &Roboto_Light_24);
    
    // Horizontal
    display.fillRect(left, height_top + height_unit*3 + height_min + 1, (rainNextHourDurationSum[8] + rainNextHourDuration[8]) * (width_5min + space_between) + 5, 1, BLACK);
    
    display.fillRect(left-3, height_top + height_unit*3 + height_min, 5, 1, BLACK);
    display.fillRect(left-3, height_top + height_unit*2 + height_min, 5, 1, BLACK);
    display.fillRect(left-3, height_top + height_unit*1 + height_min, 5, 1, BLACK);
    printText((char *)F("0"), left - 27, height_top + height_unit*3 + height_min +8, &Roboto_Light_24);
    printText((char *)F("1"), left - 27, height_top + height_unit*2 + height_min +8, &Roboto_Light_24);
    printText((char *)F("2"), left - 27, height_top + height_unit*1 + height_min +8, &Roboto_Light_24);
    
    // Vertical
    display.fillRect(left, height_top + height_unit*0.5 - 3, 1, height_unit*2.5 + height_min + 5, BLACK);

    // Vertical
    for (int i = 0; i < 9; i++) {
      display.fillRect(left + (rainNextHourDurationSum[i] + rainNextHourDuration[i]*0.5) * (width_5min + space_between) - 0.5*space_between, height_top + height_unit*3 + height_min +1, 1, 3, BLACK);
      //itoa(rainNextHour[i], cstr, 6);
      
      //printText(cstr, left + (rainNextHourDurationSum[i] + rainNextHourDuration[i]*0.5) * (width_5min + space_between) - 0.5*space_between, height_top + height_unit*3 + height_min + 30, &Roboto_Light_24); // Minutes
    }
    printText((char *)F("Mnt."), left-10, height_top + height_unit*3 + height_min + 30, &Roboto_Light_24); // Minutes
    printText((char *)F("+30min"), left + (rainNextHourDurationSum[8] + rainNextHourDuration[8])*0.5 * (width_5min + space_between) - 4*9, height_top + height_unit*3 + height_min + 30, &Roboto_Light_24); // Minutes
    printText((char *)F("+1h"), left + (rainNextHourDurationSum[8] + rainNextHourDuration[8]) * (width_5min + space_between) - 3*9, height_top + height_unit*3 + height_min + 30, &Roboto_Light_24); // Minutes
  }
  else if (drawLevel >= 1)
  {
    // Serial.print(F("Plotting rain: "));
    
    // Replace old values
    display.fillRect(left+1, height_top + height_unit, (rainNextHourDurationSum[8] + rainNextHourDuration[8]) * (width_5min + space_between),  height_unit*2 + height_min, WHITE);

    int maxRain = 0;
    for (int i = 0; i < 9; i++) {
      if(rainNextHour[i] > maxRain)
      {
        maxRain = rainNextHour[i];
      }
    }

    for (int i = 0; i < 9; i++) {
      display.fillRect(left + (rainNextHourDurationSum[i]) * (width_5min + space_between), height_top + height_unit * (3-rainNextHour[i]) + height_min,
                        rainNextHourDuration[i] * width_5min + space_between*(rainNextHourDuration[i]-1), rainNextHour[i] * height_unit + height_min, BLACK);
//      Serial.print(i);
//      Serial.print(F(": "));
//      Serial.print(rainNextHour[i]);
//      Serial.print(F(" ; "));
//      Serial.print(rainNextHourDuration[i]);
//      Serial.print(F(" ; "));
//      Serial.println(rainNextHourDurationSum[i]);
    }
    
    if (maxRain == 0)
    {
      printText((char *)F("Pas de pluie dans l'heure !"), left + 10, height_top + height_unit*2+5, &Roboto_Light_24);
    }
  }
}




////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Function for drawing Sun info
void drawSun(int drawLevel)
{
  // drawLevel: 0: Background only
  // drawLevel: 1: Background + values
  // drawlevel: 2: Values refresh

  // Sunrise and sunset
  if (drawLevel <= 1)
  {
    // Draw background
    drawSunriseIcon();
    drawSunsetIcon();
  }
  else if (drawLevel >= 1)
  {
    // Replace old values
    eraseAndPrint(nextSunrise, 335, 360, &Roboto_Light_36, BLACK, 110);
    eraseAndPrint(nextSunset, 335, 410, &Roboto_Light_36, BLACK, 110);
  }
}


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Function for drawing sensors data
void drawSensors(int drawLevel)
{
  // drawLevel: 0: Background only
  // drawLevel: 1: Background + values
  // drawlevel: 2: Values refresh

  // Defining the grid
  int rectWidth = 240;
  int left = 395;
  int left_spacing = 160;
  int rectThickness = 2;
  int rectSpacing = 50;
  int textMargin = 6;
  int height_top = 470; // 440

  if (drawLevel <= 1)
  {
    // Horizontal
//    display.fillRect(1 * rectSpacing + 0 * rectWidth, height_top + 40, rectWidth - 50, 1, BLACK);
//    display.fillRect(2 * rectSpacing + 1 * rectWidth, height_top + 40, rectWidth - 50, 1, BLACK);
//    display.fillRect(3 * rectSpacing + 2 * rectWidth, height_top + 40, rectWidth - 50, 1, BLACK);

    // Vertical
    display.fillRect(left - 10, height_top-35, 2, rectSpacing*3, BLACK);
//    display.fillRect(2 * rectSpacing + 1 * rectWidth, height_top, 2, 150, BLACK);
//    display.fillRect(3 * rectSpacing + 2 * rectWidth, height_top, 2, 150, BLACK);

    // Sensor names (fixed)
    printText(sensor1[0], left, height_top, &Roboto_Light_36); // Name
    printText(sensor2[0], left, height_top + rectSpacing, &Roboto_Light_36); // Name
    printText(sensor3[0], left, height_top + rectSpacing*2, &Roboto_Light_36); // Name
  }
  else if (drawLevel >= 1)
  {
    // Fill with white to erase old values
    display.fillRect(left + left_spacing, height_top, rectWidth, 110, WHITE);

    // Sensor #1
    printText(sensor1[1], left + left_spacing, height_top, &Roboto_Light_48); // Temperature
    printCelsiusAtCursor(&Roboto_Light_24, 24);
    printTextAtCursor((char *)F("  "), &Roboto_Light_24);
    printTextAtCursor(sensor1[3], &Roboto_Light_36);    // Humidity
    printTextAtCursor((char *)F("%"), &Roboto_Light_24);
    //printText(sensor1[2], 1 * rectSpacing + 0 * rectWidth + textMargin, height_top + textMargin + 135, &Roboto_Light_36); // Pressure
    //printTextAtCursor((char *)F("hPa / "), &Roboto_Light_24);
    //printTextAtCursor(sensor1[3], &Roboto_Light_36);    // Humidity
    //printTextAtCursor((char *)F("%"), &Roboto_Light_24);

    // Sensor #2
    printText(sensor2[1], left + left_spacing, height_top + rectSpacing, &Roboto_Light_48); // Temperature
    printCelsiusAtCursor(&Roboto_Light_24, 24);
    printTextAtCursor((char *)F("  "), &Roboto_Light_24);
    printTextAtCursor(sensor2[3], &Roboto_Light_36);    // Humidity
    printTextAtCursor((char *)F("%"), &Roboto_Light_24);
    //printText(sensor2[2], 2 * rectSpacing + 1 * rectWidth + textMargin, height_top + textMargin + 135, &Roboto_Light_36); // Pressure
    //printTextAtCursor((char *)F("hPa / "), &Roboto_Light_24);
    //printTextAtCursor(sensor2[3], &Roboto_Light_36);    // Humidity
    //printTextAtCursor((char *)F("%"), &Roboto_Light_24);

    // Sensor #3
    printText(sensor3[1], left + left_spacing, height_top + rectSpacing*2, &Roboto_Light_48); // Temperature
    printCelsiusAtCursor(&Roboto_Light_24, 24);
    printTextAtCursor((char *)F("  "), &Roboto_Light_24);
    printTextAtCursor(sensor3[3], &Roboto_Light_36);    // Humidity
    printTextAtCursor((char *)F("%"), &Roboto_Light_24);
    //printText(sensor3[2], 3 * rectSpacing + 2 * rectWidth + textMargin, height_top + textMargin + 135, &Roboto_Light_36); // Pressure
    //printTextAtCursor((char *)F("hPa / "), &Roboto_Light_24);
    //printTextAtCursor(sensor3[3], &Roboto_Light_36);    // Humidity
    //printTextAtCursor((char *)F("%"), &Roboto_Light_24);
  }
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Function for drawing weather info
void drawCities(int drawLevel)
{
  // drawLevel: 0: Background only
  // drawLevel: 1: Background + values
  // drawlevel: 2: Values refresh

  int left = 650;
  int space = 125;

  // Other cities forecasts
  if (drawLevel <= 1)
  {
    // Draw background
    // Spacers
    // Vertical
    display.fillRect(500, 80, 2, 310, BLACK);

    // Horizontal
    display.fillRect(left - 50, 175, 100, 1, BLACK);
    display.fillRect(left - 50, 175 + space, 100, 1, BLACK);

    // Cities' names

    //printText(city1[0], left - 20, 90, &Roboto_Light_36);
    //printText(city2[0], left - 20, 90 + space, &Roboto_Light_36);
    //printText(city3[0], left - 20, 90 + space * 2, &Roboto_Light_36);
  }
  else if (drawLevel >= 1)
  {
    // Fill with white to erase old values
    display.fillRect(left, 91, 110, 83, WHITE); // City1
    display.fillRect(left, 91 + space, 110, 83, WHITE); // City2
    display.fillRect(left, 91 + space * 2, 110, 83, WHITE); // City3

    // Cities' names - Here because some letters are erased otherwise
    printText(city1[0], left - 20, 90, &Roboto_Light_36);
    printText(city2[0], left - 20, 90 + space, &Roboto_Light_36);
    printText(city3[0], left - 20, 90 + space * 2, &Roboto_Light_36);

    // City 1
    drawWeatherIcon_small(left - 120, 55, city1[3]);
    printText(city1[1], left, 125, &Roboto_Light_36);
    printCelsiusAtCursor(&Roboto_Light_24, 24);
    printText(city1[2], left, 160, &Roboto_Light_36);
    printCelsiusAtCursor(&Roboto_Light_24, 24);

    // City 2
    drawWeatherIcon_small(left - 120, 55 + space, city2[3]);
    printText(city2[1], left, 125 + space, &Roboto_Light_36);
    printCelsiusAtCursor(&Roboto_Light_24, 24);
    printText(city2[2], left, 160 + space, &Roboto_Light_36);
    printCelsiusAtCursor(&Roboto_Light_24, 24);

    // City 3
    drawWeatherIcon_small(left - 120, 55 + space * 2, city3[3]);
    printText(city3[1], left, 125 + space * 2, &Roboto_Light_36);
    printCelsiusAtCursor(&Roboto_Light_24, 24);
    printText(city3[2], left, 160 + space * 2, &Roboto_Light_36);
    printCelsiusAtCursor(&Roboto_Light_24, 24);
  }
}
