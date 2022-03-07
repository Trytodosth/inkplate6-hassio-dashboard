/*
  Network.cpp
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

// Network.cpp contains various functions and classes that enable Weather station
// They have been declared in seperate file to increase readability

#include "Network.h"
#include "Secrets.h" // WIFI credentials and Hassio parameters

#include "City.h"
#include "WeatherForecast.h"
#include "WeatherNow.h"
#include "Room.h"
#include "Field.h"

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include <ArduinoJson.h>

#include "Binary_images/no_wifi_icon.h"

// Static Json from ArduinoJson library
StaticJsonDocument<32000> doc;

void Network::begin()
{
  // Initiating wifi, like in BasicHttpClient example
  WiFi.mode(WIFI_STA);
  WiFi.begin(WiFi_ssid, WiFi_pass);

  int cnt = 0;
  Serial.print(F("Waiting for WiFi to connect..."));
  while ((WiFi.status() != WL_CONNECTED))
  {
    Serial.print(F("."));
    delay(1000);
    ++cnt;

    if (cnt == 20)
    {
      Serial.println("Can't connect to WIFI, restarting");
      print_no_wifi();
      ESP.restart();
    }
  }
  Serial.println(F(" connected"));

  // Find internet time
  setTime();

  // reduce power by making WiFi module sleep
  //WiFi.setSleep(1);
}

void Network::CheckWiFi(int nbMaxAttempts)
{
  // If not connected to wifi reconnect wifi
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.reconnect();

    delay(5000);

    int cnt = 0;
    Serial.println(F("Waiting for WiFi to reconnect..."));
    while ((WiFi.status() != WL_CONNECTED))
    {
      // Prints a dot every second that wifi isn't connected
      Serial.print(F("."));
      delay(1000);
      ++cnt;

      if (cnt == nbMaxAttempts)
      {
        Serial.println(F("Can't connect to WIFI, restarting"));
        print_no_wifi();
        ESP.restart();
      }
    }
    Serial.println(F(" connected"));
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

bool Network::getSensorsData(char *sensor1_temp, char *sensor1_press, char *sensor1_hum,
                             char *sensor2_temp, char *sensor2_press, char *sensor2_hum,
                             char *sensor3_temp, char *sensor3_press, char *sensor3_hum)
{
  bool f = 0;
  CheckWiFi();

  // Wake up if sleeping and save inital state
  bool sleep = WiFi.getSleep();
  WiFi.setSleep(false);

  // Temporary
  char friendly_name[24]; // Not used, too many variables
  char unit[8];           // Not used, too many variables

  Serial.println(F("Initiating data collection from HA server"));

  // Fetching sensors data
  getSensorData("sensor.cuisine_pression", friendly_name, sensor1_press, unit);
  getSensorData("sensor.cuisine_humidite", friendly_name, sensor1_hum, unit);
  getSensorData("sensor.cuisine_temperature", friendly_name, sensor1_temp, unit);

  // Fetching sensors data
  getSensorData("sensor.salon_pression", friendly_name, sensor2_press, unit);
  getSensorData("sensor.salon_humidite", friendly_name, sensor2_hum, unit);
  getSensorData("sensor.salon_temperature", friendly_name, sensor2_temp, unit);

  // Fetching sensors data
  getSensorData("sensor.chambre_pression", friendly_name, sensor3_press, unit);
  getSensorData("sensor.chambre_humidite", friendly_name, sensor3_hum, unit);
  getSensorData("sensor.chambre_temperature", friendly_name, sensor3_temp, unit);

  // TODO: Check to be integrated
  f = 0;

  // Return to initial state
  WiFi.setSleep(sleep);

  return !f;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

bool Network::getNextRainData(int &rain0min, int &rain5min, int &rain10min, int &rain15min, int &rain20min, int &rain25min,
                              int &rain35min, int &rain45min, int &rain55min)
{
  bool f = 0;
  CheckWiFi();

  // Wake up if sleeping and save inital state
  bool sleep = WiFi.getSleep();
  WiFi.setSleep(false);

  Serial.println(F("Fetching next rain"));

  bool success;
  success = getJSON("sensor.val_d_epy_next_rain");
  if (!success)
    return success;

  rain0min = getRainIntensity(doc["attributes"]["1_hour_forecast"]["0 min"].as<char *>());
  rain5min = getRainIntensity(doc["attributes"]["1_hour_forecast"]["5 min"].as<char *>());
  rain10min = getRainIntensity(doc["attributes"]["1_hour_forecast"]["10 min"].as<char *>());
  rain15min = getRainIntensity(doc["attributes"]["1_hour_forecast"]["15 min"].as<char *>());
  rain20min = getRainIntensity(doc["attributes"]["1_hour_forecast"]["20 min"].as<char *>());
  rain25min = getRainIntensity(doc["attributes"]["1_hour_forecast"]["25 min"].as<char *>());
  rain35min = getRainIntensity(doc["attributes"]["1_hour_forecast"]["35 min"].as<char *>());
  rain45min = getRainIntensity(doc["attributes"]["1_hour_forecast"]["45 min"].as<char *>());
  rain55min = getRainIntensity(doc["attributes"]["1_hour_forecast"]["55 min"].as<char *>());

  // TODO: Check to be integrated
  f = 0;

  // Return to initial state
  WiFi.setSleep(sleep);

  return !f;
}

int Network::getRainIntensity(const char *rainString)
{
  if (strcmp(rainString, "Temps sec") == 0)
  {
    return 0;
  }
  if (strcmp(rainString, "Pluie faible") == 0)
  {
    return 1;
  }
  if (strcmp(rainString, "Pluie modérée") == 0)
  {
    return 2;
  }
  if (strcmp(rainString, "Pluie forte") == 0)
  {
    return 3;
  }

  Serial.print(F("Error next Rain: "));
  Serial.println(rainString);
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

// Gets the JSON associated to an API request. Saves it in the doc object
// Careful, each request is time / energy consumming !
bool Network::getJSON(const char entityName[])
{
  bool success = false;
  doc.clear();
  // Http object used to make get request
  HTTPClient http;

  // Initialization
  http.getStream().setNoDelay(true);
  http.getStream().setTimeout(1);
  char url[256];
  sprintf(url, "%s/states/%s", HAserverAPI, entityName);

  // Initiate http
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", HAtoken);

  // Actually do request
  int httpCode = http.GET();
  // Serial.println(httpCode);
  if (httpCode == 200)
  {
    // Try parsing JSON object
    DeserializationError error = deserializeJson(doc, http.getStream());

    // If an error happens print it to Serial monitor
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(entityName);
      Serial.println(error.c_str());
    }
    else
    {
      // Everything is OK, JSON extracted, do your stuff
      success = true;
    }
  }
  else if (httpCode < 0)
  {
    Serial.println(F("Negative HTTP code (internal error)"));
  }
  else
  {
    display.clearDisplay();
    display.setCursor(50, 290);
    if (httpCode == 401)
    {
      display.println(F("Network error (401), probably wrong API key"));
    }
    else
    {
      display.println(F("HTTP code not 200"));
    }
    display.setCursor(50, 330);
    display.print(F("HTTP code: "));
    display.println(httpCode);
    display.setCursor(50, 370);
    display.print(F("Trying to get entity: "));
    display.println(entityName);
    display.display();
    delay(5000);
    display.clearDisplay();
    //drawLevel = 1; // Next calls will redraw background we just removed! (+ values)
  }

  // Stop http
  http.end();

  return success;
}

// Gets the state and save is in output
bool Network::getState(char entityName[], char *output)
{
  bool success;
  success = getJSON(entityName);
  if (!success)
    return success;

  // Extracting Info from JSON
  strcpy(output, doc["state"].as<char *>());
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

bool Network::getSensorData(char entityName[], char *friendlyName, char *value, char *unit)
{
  bool success;
  success = getJSON(entityName);
  if (!success)
    return success;

  // Extracting Info from JSON
  strcpy(friendlyName, doc["attributes"]["friendly_name"].as<char *>());
  strcpy(value, doc["state"].as<char *>());
  strcpy(unit, doc["attributes"]["unit_of_measurement"].as<char *>());
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

bool Network::getField(Field &field)
{
  if (!field.updateNeeded())
    return true;

  bool success;
  success = getJSON(field.ApiID);
  Serial.print("Trying to fetch weather data from ");
  Serial.print(field.Name);
  Serial.print(" at ");
  Serial.println(field.ApiID);

  if (!success)
    return false;

  field.setValue(doc["state"].as<char *>());

  Serial.print ("Read value: ");
  Serial.println(field.Value);
  return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

bool Network::getCityWeather(City &city)
{
  if (!city.updateNeeded())
    return true;

  bool success;
  success = getJSON(city.ApiID);
  Serial.print("Trying to fetch weather data from ");
  Serial.print(city.Name);
  Serial.print(" at ");
  Serial.println(city.ApiID);

  if (!success)
    return false;

  Serial.println("Extracting current weather");
  city.ActualWeather.setWeather(doc["state"].as<char *>());
  city.ActualWeather.setTemperature(doc["attributes"]["temperature"].as<double>());
  city.ActualWeather.setWindSpeed(doc["attributes"]["wind_speed"].as<double>());
  //dtostrf(doc["attributes"]["forecast"][0]["precipitation"].as<double>(), 4, 1, expectedRain);
  //iconAbbr(currentWeatherIcon, doc["state"].as<char *>());

  // Extracting Info from JSON
  for (size_t indForecast = 0; indForecast < 4; indForecast++)
  {
    // Serial.print("Extracting forecast #");
    // Serial.println(indForecast);

    city.Forecasts[indForecast].setWeather(doc["attributes"]["forecast"][indForecast]["condition"].as<char *>());
    //city.Forecasts[indForecast].setDay(doc["attributes"]["forecast"][indForecast]["condition"].as<char *>());
    city.Forecasts[indForecast].setDayFromDate(doc["attributes"]["forecast"][indForecast]["datetime"].as<char *>());
    city.Forecasts[indForecast].setTemperatureMax(doc["attributes"]["forecast"][indForecast]["temperature"].as<float>());
    city.Forecasts[indForecast].setTemperatureMin(doc["attributes"]["forecast"][indForecast]["templow"].as<float>());
    city.Forecasts[indForecast].setRainHeight(doc["attributes"]["forecast"][indForecast]["precipitation"].as<float>());
    city.Forecasts[indForecast].setWindSpeed(doc["attributes"]["forecast"][indForecast]["wind_speed"].as<float>());
  }
  //dtostrf(doc["attributes"]["forecast"][indForecast]["temperature"].as<double>(), 4, 1, forecastTemperatureMax);

  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

void Network::setTime()
{
  // Used for setting correct time
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2)
  {
    // Print a dot every half a second while time is not set
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();

  // Used to store time info
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);

  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

void Network::print_no_wifi()
{
  display.clearDisplay(); // Clear Buffer

  // 1bit mode: display.drawBitmap(pos_x, pos_y, progmem_const, width, height, color=BLACK | WHITE, color_bg = BLACK | WHITE);
  display.drawBitmap(120, 220, no_wifi_icon, 200, 167, WHITE, BLACK);

  // Text
  display.setTextColor(BLACK);
  display.setFont(&Roboto_Light_48);
  display.setCursor(330, 280);
  display.print(F("WiFi non trouvee"));
  display.setFont(&Roboto_Light_36);
  display.setCursor(330, 330);
  display.print(F("Recherche > "));
  display.print(WiFi_ssid);
  display.setCursor(330, 370);
  display.print(F("Redemarrage dans 5 sec"));

  display.display();
  delay(5000);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

void Network::iconAbbr(char *iconOutput, const char *weatherState)
{
  // Here are made the correspondances between the "weather" integration and our icons names
  // https://www.home-assistant.io/integrations/weather/#condition-mapping
  // In tegration like MEteo-France use the same "weather" so it's compatible.
  // https://github.com/home-assistant/core/blob/e52542c4d7a33ad051130e5ebddb4178fd570e54/homeassistant/components/meteo_france/const.py

  // Very simply:
  strcpy(iconOutput, weatherState);

  // If you use anoth integration, should should make the names correspondances there:
  //    if (strcmp(weatherState, "clear-night") == 0)
  //      strcpy(iconOutput, "clear-night");
  //    if (strcmp(weatherState, "cloudy") == 0)
  //      strcpy(iconOutput, "cloudy");
  //    if (strcmp(weatherState, "fog") == 0)
  //      strcpy(iconOutput, "fog");
  //    if (strcmp(weatherState, "hail") == 0)
  //      strcpy(iconOutput, "hail");
  //    if (strcmp(weatherState, "lightning") == 0)
  //      strcpy(iconOutput, "lightning");
  //    if (strcmp(weatherState, "lightning-rainy") == 0)
  //      strcpy(iconOutput, "lightning-rainy");
  //    if (strcmp(weatherState, "partlycloudy") == 0)
  //      strcpy(iconOutput, "partlycloudy");
  //    if (strcmp(weatherState, "pouring") == 0)
  //      strcpy(iconOutput, "pouring");
  //    if (strcmp(weatherState, "rainy") == 0)
  //      strcpy(iconOutput, "rainy");
  //    if (strcmp(weatherState, "snowy") == 0)
  //      strcpy(iconOutput, "snowy");
  //    if (strcmp(weatherState, "snowy-rainy") == 0)
  //      strcpy(iconOutput, "snowy-rainy");
  //    if (strcmp(weatherState, "sunny") == 0)
  //      strcpy(iconOutput, "sunny");
  //    if (strcmp(weatherState, "windy") == 0)
  //      strcpy(iconOutput, "windy");
  //    if (strcmp(weatherState, "windy-variant") == 0)
  //      strcpy(iconOutput, "windy-variant");
}
