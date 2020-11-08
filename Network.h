/*
Network.h
Inkplate 6 Arduino library
David Zovko, Borna Biro, Denis Vajak, Zvonimir Haramustek @ e-radionica.com
September 24, 2020
https://github.com/e-radionicacom/Inkplate-6-Arduino-library

For support, please reach over forums: forum.e-radionica.com/en
For more info about the product, please check: www.inkplate.io

This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
Please review the LICENSE file included with this example.
If you have any questions about licensing, please contact techsupport@e-radionica.com
Distributed as-is; no warranty is given.
*/

#include "Arduino.h"

#include "Inkplate.h"
#include <WiFi.h>

#include <HTTPClient.h>

#include <WiFiClientSecure.h>

// To get timeZone from main file
extern int timeZone;

// wifi ssid and password
extern char *ssid;
extern char *pass;

extern char *lon;
extern char *lat;

extern char *apiKey;
extern char *HAtoken;
extern char *HAserverAPI;

extern Inkplate display;

#ifndef NETWORK_H
#define NETWORK_H

// All functions defined in Network.cpp

class Network
{
  public:
    // Functions we can access in main file
    void begin(char *city);
    bool getData(char *currentTime, char *currentTemp, char *currentWind, char *currentWeather, char *currentWeatherIcon, char *nextSunrise, char *nextSunset,
                      char *sensor1_temp, char *sensor1_press, char *sensor1_hum,
                      char *sensor2_temp, char *sensor2_press, char *sensor2_hum,
                      char *sensor3_temp, char *sensor3_press, char *sensor3_hum,
                      char *loc1_temp_max, char *loc1_temp_min, char *loc1_icon, char *loc2_temp_max, char *loc2_temp_min, char *loc2_icon, char *loc3_temp_max, char *loc3_temp_min, char *loc3_icon);
    void getState(char entityName[], char *output);
    void getSensorData(char entityName[], char *sensor1_name, char *value, char *unit);
    void getWeatherHome(char *currentTemp, char *currentWind, char *currentWeather, char *currentWeatherIcon);
    void getWeatherForecast(char entityName[], int indForecast, char *forecastTemperatureMax, char *forecastTemperatureMin, char *forecastIcon);
    void iconAbbr(char *iconOutput, const char *weatherState);

    // Used for storing retrieved data timestamp
    time_t dataEpoch = 0;

  private:
    // Functions called from within our class
    void setTime();
};

#endif
