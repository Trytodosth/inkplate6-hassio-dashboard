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

#include <Arduino.h>

#include <Inkplate.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#include "City.h"
#include "Field.h"

extern const GFXfont Roboto_Light_36;
extern const GFXfont Roboto_Light_48;

// WiFi SSID and Password
extern char *WiFi_ssid;
extern char *WiFi_pass;

// Home Assistant Information
extern char *apiKey;
extern char *HAtoken;
extern char *HAserverAPI;

extern Inkplate display;

extern int drawLevel;

#ifndef NETWORK_H
#define NETWORK_H

// All functions defined in Network.cpp

class Network
{
public:
  // Functions we can access in main file
  void begin();
  void CheckWiFi(int nbMaxAttempts = 10);
  bool getSensorsData(char *sensor1_temp, char *sensor1_press, char *sensor1_hum,
                      char *sensor2_temp, char *sensor2_press, char *sensor2_hum,
                      char *sensor3_temp, char *sensor3_press, char *sensor3_hum);
  bool getNextRainData(int &rain0min, int &rain5min, int &rain10min, int &rain15min, int &rain20min, int &rain25min,
                       int &rain35min, int &rain45min, int &rain55min);
  void iconAbbr(char *iconOutput, const char *weatherState);

  bool getCityWeather(City &city);
  bool getField(Field &field);

  // Used for storing retrieved data timestamp
  time_t dataEpoch = 0;

private:
  bool getJSON(const char entityName[]);
  bool getState(char entityName[], char *output);
  bool getSensorData(char entityName[], char *sensor1_name, char *value, char *unit);
  int getRainIntensity(const char *rainString);
  // Functions called from within our class
  void setTime();
  void print_no_wifi();
};

#endif
