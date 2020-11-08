/*
   Home Assistant Hub
   An Inkplate 6 personal project to display Home Assistant Data
   using the HA API through HTTP calls
   * HA setup required to work!

   Thi work was hugely based on the e-Radionica tutorial: Real time Weather station

   For this example you will need only USB cable and Inkplate 6.
   Select "Inkplate 6(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6(ESP32)" option? Follow the tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/


   IMPORTANT:
   
*/

/*
   Icons made by Freepik [https://www.flaticon.com/authors/freepik]
   More on Flaticon [www.flaticon.com]
*/

//---------- CHANGE HERE  -------------:

// Time zone for adding hours
int timeZone = 2;

// City name to de displayed on the bottom
char city[128] = "Val-d'Epy";

// Coordinates sent to the api
char *lon = "46.3833";
char *lat = "5.4167";

// Change to your wifi ssid and password
char *ssid = "Sunrise_2.4GHz_156F5C";
char *pass = "W4vpx7e2k2WL";

// Change to your api key, if you don't have one, head over to:
// https://openweathermap.org/guide , register and copy the key provided
char *OWMapiKey = "b506ac6f0864d4abd7187a86de5c4329";
char *HAserverAPI = "http://192.168.1.33/api";
char *HAtoken = "Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiIyNmI1YTc4MzFkNmM0MTExOWY0MGVkNmYwZjhlM2YyZSIsImlhdCI6MTYwMzkxMjU0MCwiZXhwIjoxOTE5MjcyNTQwfQ.3UWoLwe7k-pEqZ5S9paHNV0W1B_Cvh01i2fNvTxo1bU";

//----------------------------------

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Header file for easier code readability
#include "Network.h"

// Including fonts used
#include "Fonts/Roboto_Light_24.h"
#include "Fonts/Roboto_Light_36.h"
#include "Fonts/Roboto_Light_48.h"
#include "Fonts/Roboto_Light_72.h"
//#include "Fonts/Roboto_Light_120.h"

// Including icons generated by the py file
#include "icons.h"

// Delay between refreshes. No limit from HA, just think of relevantness and battery life!
#define DELAY_S 60L

// Inkplate object
Inkplate display(INKPLATE_3BIT);

// All our network functions are in this object, see Network.h
Network network;


// Contants used for drawing icons
char abbrs[32][32] = {"clear-night", "cloudy", "fog", "hail", "lightning", "lightning-rainy", "partlycloudy",
                      "pouring", "rainy", "snowy", "snowy-rainy", "sunny", "windy", "windy-variant", "exceptional"
                     };

// Logo associated with keywords. You can use several times the same (here windy-variant = windy)
const uint8_t *logos_100px[15] = {
  icon_clear_night_100px, icon_cloudy_100px, icon_fog_100px, icon_hail_100px, icon_lightning_rainy_100px, icon_lightning_100px, icon_partlycloudy_100px,
  icon_pouring_100px, icon_rainy_100px, icon_snowy_100px, icon_snowy_rainy_100px, icon_sunny_100px, icon_windy_100px, icon_windy_100px, icon_exceptional_100px
};
const uint8_t *logos_200px[15] = {
  icon_clear_night_200px, icon_cloudy_200px, icon_fog_200px, icon_hail_200px, icon_lightning_rainy_200px, icon_lightning_200px, icon_partlycloudy_200px,
  icon_pouring_200px, icon_rainy_200px, icon_snowy_200px, icon_snowy_rainy_200px, icon_sunny_200px, icon_windy_200px, icon_windy_200px, icon_exceptional_200px
};

// Variables for storing sensors data
char sensor1[4][12] = {
  "Cuisine",
  "21.2",
  "1032",
  "44.0",
};
char sensor2[4][12] = {
  "Chambre",
  "21.2",
  "1032",
  "44.0",
};
char sensor3[4][12] = {
  "Grange",
  "21.2",
  "1032",
  "44.0",
};

// Variables for storing cities strings
char city1[4][12] = {
  "Lyon",
  "21.2",
  "5.2",
  "02d",
};
char city2[4][12] = {
  "Suhr",
  "21.2",
  "5.2",
  "02d",
};
char city3[4][12] = {
  "Chambly",
  "21.2",
  "5.2",
  "02d",
};

// Variable for counting partial refreshes
long refreshes = 0;

// Constant to determine when to full update
const int fullRefresh = 10;

// Variables for storing current time and weather info
char currentTime[128] = "Not set";
char currentTemp[16] = "99.9";
char currentWind[16] = "99";
char nextSunrise[6] = "0h00";
char nextSunset[6] = "24h00";
char currentWeather[128] = "Weather not set";
char currentWeatherIcon[20] = "exceptional";

//char abbr1[16];
//char abbr2[16];
//char abbr3[16];
//char abbr4[16];

// functions defined below
void drawWeather();
void drawSensors();
void drawCity();
void drawTime();
void drawCities();
void printCelsiusCircle(int fontsize_px, int color);

void setup()
{
  // Begin serial and display
  Serial.begin(115200);
  display.begin();

  // Initial cleaning of buffer and physical screen
  display.clearDisplay();
  display.clean();
  display.setTextColor(0, 7);
  display.setTextWrap(false);

  // Calling our begin from network.h file
  network.begin(city);

  // Welcome screen
  //display.setTextSize(4);
  display.setFont(&Roboto_Light_48);
  display.setCursor(80, 260);
  display.print(F("Home Assistant Hub"));
  display.display();
  delay(1000);
  //display.setTextSize(3);
  display.setFont(&Roboto_Light_36);
  display.setCursor(80, 300);
  display.print(F("> powered by an Inkplate 6"));
  display.display();
  delay(1000);
  display.setCursor(80, 340);
  display.print(F("> created by Maxime"));
  display.display();
  //display.partialUpdate(); //Not working since some changes....

  // Wait a bit before proceeding
  delay(2000);
}

void loop()
{
  // Clear display
  display.clearDisplay();

  // Get all relevant data, see Network.cpp for info
  //network.getTime(currentTime);
  if (refreshes % fullRefresh == 0)
  {
    while (!network.getData(currentTime, currentTemp, currentWind, currentWeather, currentWeatherIcon, nextSunrise, nextSunset,
                            sensor1[1], sensor1[2], sensor1[3],
                            sensor2[1], sensor2[2], sensor2[3],
                            sensor3[1], sensor3[2], sensor3[3],
                            city1[1], city1[2], city1[3], city2[1], city2[2], city2[3], city3[1], city3[2], city3[3]))
    {
      Serial.println("Retrying fetching data!");
      delay(5000);
    }
    //network.getHours(hours[0], hours[1], hours[2], hours[3]);
  }

  // Draw data, see functions below for info
  drawWeather();
  drawCity();
  drawTime();
  drawCities();
  drawSensors();

  Serial.println(sensor1[0]);
  Serial.println(sensor3[0]);
  Serial.println(sensor2[0]);

  // Refresh full screen every fullRefresh times, defined above
  if (refreshes % fullRefresh == 0)
    display.display();
  else
    display.partialUpdate();

  // Go to sleep before checking again
  esp_sleep_enable_timer_wakeup(1000000L * DELAY_S);
  (void)esp_light_sleep_start();
  ++refreshes;
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Function for drawing weather info
void drawWeather()
{
  // Searching for weather state abbreviation
  for (int i = 0; i < 18; ++i)
  {
    // If found draw specified icon
    if (strcmp(abbrs[i], currentWeatherIcon) == 0)
      display.drawBitmap3Bit(30, 30, logos_200px[i], 200, 200);
  }

  display.setTextColor(BLACK, WHITE);

  // Weather text
  //display.setFont(&Roboto_Light_36);
  //display.setTextSize(1);
  //display.setCursor(40, 200);
  //display.println(currentWeather);

  // Temperature
  display.setFont(&Roboto_Light_72);
  display.setCursor(40, 300);
  display.print(currentTemp);
  display.setFont(&Roboto_Light_36);
  display.print(F("°C"));

  // Wind
  display.setFont(&Roboto_Light_36);
  display.setCursor(105, 348);
  display.print(currentWind);
  display.setFont(&Roboto_Light_24);
  display.print(F("m/s"));
  display.drawBitmap(50, 313, icon_Wind, 50, 50, BLACK);

  // Sunrise and sunset
  display.setFont(&Roboto_Light_36);
  display.setCursor(65, 410);
  display.print(nextSunrise);
  display.setCursor(215, 410);
  display.print(nextSunset);
  display.drawBitmap(10, 377, icon_Sunrise_50px, 50, 41, BLACK);
  display.drawBitmap(160, 377, icon_Sunset_50px, 50, 41, BLACK);
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Function for drawing current time
void drawTime()
{
  // Drawing current time
  display.setTextColor(BLACK, WHITE);
  display.setFont(&Roboto_Light_24);
  display.setTextSize(1);

  display.setCursor(560, 30);
  display.println(currentTime);
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Function for drawing city name
void drawCity()
{
  // Drawing city name
  display.setTextColor(BLACK, WHITE);
  display.setFont(&Roboto_Light_48);
  display.setTextSize(1);

  display.setCursor(800 / 3 - 10 * strlen(city), 45);
  display.println(city);
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Function for drawing sensors data
void drawSensors()
{
  // Drawing 4 black rectangles in which temperatures will be written
  int rectWidth = 240;
  int rectThickness = 2;
  int rectSpacing = (800 - rectWidth * 3) / 4;

  display.fillRect(1 * rectSpacing + 0 * rectWidth, 440, rectWidth, 150, 6);
  display.fillRect(2 * rectSpacing + 1 * rectWidth, 440, rectWidth, 150, 6);
  display.fillRect(3 * rectSpacing + 2 * rectWidth, 440, rectWidth, 150, 6);

  // Horizontal
  display.fillRect(1 * rectSpacing + 0 * rectWidth, 480, rectWidth - 50, 1, BLACK);
  display.fillRect(2 * rectSpacing + 1 * rectWidth, 480, rectWidth - 50, 1, BLACK);
  display.fillRect(3 * rectSpacing + 2 * rectWidth, 480, rectWidth - 50, 1, BLACK);

  // Vertical
  display.fillRect(1 * rectSpacing + 0 * rectWidth, 440, 2, 150, BLACK);
  display.fillRect(2 * rectSpacing + 1 * rectWidth, 440, 2, 150, BLACK);
  display.fillRect(3 * rectSpacing + 2 * rectWidth, 440, 2, 150, BLACK);


  int textMargin = 6;

  display.setTextSize(1);

  // Sensor #1
  display.setFont(&Roboto_Light_36);
  display.setCursor(1 * rectSpacing + 0 * rectWidth + textMargin, 440 + textMargin + 30);
  display.println(sensor1[0]);

  display.setCursor(1 * rectSpacing + 0 * rectWidth + textMargin, 440 + textMargin + 85);
  display.setFont(&Roboto_Light_48);
  display.print(sensor1[1]);
  display.setFont(&Roboto_Light_24);
  printCelsiusCircle(24, 0);
  display.println(F("C"));

  display.setCursor(1 * rectSpacing + 0 * rectWidth + textMargin, 440 + textMargin + 135);
  display.setFont(&Roboto_Light_36);
  display.print(sensor1[2]);
  display.setFont(&Roboto_Light_24);
  display.print(F("hPa / "));

  display.setFont(&Roboto_Light_36);
  display.print(sensor1[3]);
  display.setFont(&Roboto_Light_24);
  display.println(F("%"));

  // Sensor #2
  display.setFont(&Roboto_Light_36);
  display.setCursor(2 * rectSpacing + 1 * rectWidth + textMargin, 440 + textMargin + 30);
  display.println(sensor2[0]);

  display.setCursor(2 * rectSpacing + 1 * rectWidth + textMargin, 440 + textMargin + 85);
  display.setFont(&Roboto_Light_48);
  display.print(sensor2[1]);
  display.setFont(&Roboto_Light_24);
  printCelsiusCircle(24, 0);
  display.println(F("C"));

  display.setCursor(2 * rectSpacing + 1 * rectWidth + textMargin, 440 + textMargin + 135);
  display.setFont(&Roboto_Light_36);
  display.print(sensor2[2]);
  display.setFont(&Roboto_Light_24);
  display.print(F("hPa / "));

  display.setFont(&Roboto_Light_36);
  display.print(sensor2[3]);
  display.setFont(&Roboto_Light_24);
  display.println(F("%"));

  // Sensor #3
  display.setFont(&Roboto_Light_36);
  display.setCursor(3 * rectSpacing + 2 * rectWidth + textMargin, 440 + textMargin + 30);
  display.println(sensor3[0]);

  display.setCursor(3 * rectSpacing + 2 * rectWidth + textMargin, 440 + textMargin + 85);
  display.setFont(&Roboto_Light_48);
  display.print(sensor3[1]);
  display.setFont(&Roboto_Light_24);
  printCelsiusCircle(24, 0);
  display.println(F("C"));

  display.setCursor(3 * rectSpacing + 2 * rectWidth + textMargin, 440 + textMargin + 135);
  display.setFont(&Roboto_Light_36);
  display.print(sensor3[2]);
  display.setFont(&Roboto_Light_24);
  display.print(F("hPa / "));

  display.setFont(&Roboto_Light_36);
  display.print(sensor3[3]);
  display.setFont(&Roboto_Light_24);
  display.println(F("%"));
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Function for drawing weather info
void drawCities()
{
  int left = 650;
  int space = 125;

  // City 1
  for (int i = 0; i < 18; ++i)
  {
    // If found draw specified icon
    if (strcmp(abbrs[i], city1[3]) == 0)
      display.drawBitmap3Bit(left - 120, 55, logos_100px[i], 100, 100);
  }
  display.setFont(&Roboto_Light_36);
  display.setTextSize(1);
  display.setCursor(left - 20, 90);
  display.println(city1[0]);
  display.setCursor(left, 125);
  display.setFont(&Roboto_Light_36);
  display.print(city1[1]);
  display.setFont(&Roboto_Light_24);
  printCelsiusCircle(24, 0);
  display.println(F("C"));
  display.setCursor(left, 160);
  display.setFont(&Roboto_Light_36);
  display.print(city1[2]);
  display.setFont(&Roboto_Light_24);
  printCelsiusCircle(24, 0);
  display.println(F("C"));;


  // City 2
  for (int i = 0; i < 18; ++i)
  {
    // If found draw specified icon
    if (strcmp(abbrs[i], city2[3]) == 0)
      display.drawBitmap3Bit(left - 120, 55 + space, logos_100px[i], 100, 100);
  }
  display.setFont(&Roboto_Light_36);
  display.setTextSize(1);
  display.setCursor(left - 20, 90 + space);
  display.println(city2[0]);
  display.setCursor(left, 125 + space);
  display.setFont(&Roboto_Light_36);
  display.print(city2[1]);
  display.setFont(&Roboto_Light_24);
  printCelsiusCircle(24, 0);
  display.println(F("C"));
  display.setCursor(left, 160 + space);
  display.setFont(&Roboto_Light_36);
  display.print(city2[2]);
  display.setFont(&Roboto_Light_24);
  printCelsiusCircle(24, 0);
  display.println(F("C"));


  // City 3
  for (int i = 0; i < 18; ++i)
  {
    // If found draw specified icon
    if (strcmp(abbrs[i], city3[3]) == 0)
      display.drawBitmap3Bit(left - 120, 55 + space * 2, logos_100px[i], 100, 100);
  }
  display.setFont(&Roboto_Light_36);
  display.setTextSize(1);
  display.setCursor(left - 20, 90 + space * 2);
  display.println(city3[0]);
  display.setCursor(left, 125 + space * 2);
  display.setFont(&Roboto_Light_36);
  display.print(city3[1]);
  display.setFont(&Roboto_Light_24);
  printCelsiusCircle(24, 0);
  display.println(F("C"));
  display.setCursor(left, 160 + space * 2);
  display.setFont(&Roboto_Light_36);
  display.print(city3[2]);
  display.setFont(&Roboto_Light_24);
  printCelsiusCircle(24, 0);
  display.println(F("C"));



  // Spacers
  // Vertical
  display.fillRect(500, 80, 2, 310, BLACK);

  // Horizontal
  display.fillRect(left - 50, 175, 100, 1, BLACK);
  display.fillRect(left - 50, 175 + space, 100, 1, BLACK);
  //display.drawBitmap(left-80, 70, logos[i], 48, 48, BLACK);
}

void printCelsiusCircle(int fontsize_px, int color)
{
  display.drawCircle(display.getCursorX() + fontsize_px * 0.15, display.getCursorY() - fontsize_px * 0.6, fontsize_px * 0.125, color);
  display.setCursor(display.getCursorX() + fontsize_px * 0.3, display.getCursorY());
}
