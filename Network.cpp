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

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include <ArduinoJson.h>

// Static Json from ArduinoJson library
StaticJsonDocument<32000> doc;

void Network::begin()
{
    // Initiating wifi, like in BasicHttpClient example
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);

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
            delay(100);
            ESP.restart();
        }
    }
    Serial.println(F(" connected"));

    // Find internet time
    setTime();

    // reduce power by making WiFi module sleep
    WiFi.setSleep(1);
}


bool Network::getData(char *currentTime, char *currentTemp, char *currentWind, char *currentWeather, char *currentWeatherIcon, char *expectedRain, char *nextSunrise, char *nextSunset,
                      char *today_temp_max, char *today_temp_min, char *today_icon, char *tomorr_temp_max, char *tomorr_temp_min, char *tomorr_icon, 
                      char *sensor1_temp, char *sensor1_press, char *sensor1_hum,
                      char *sensor2_temp, char *sensor2_press, char *sensor2_hum,
                      char *sensor3_temp, char *sensor3_press, char *sensor3_hum,
                      char *loc1_temp_max, char *loc1_temp_min, char *loc1_icon, char *loc2_temp_max, char *loc2_temp_min, char *loc2_icon, char *loc3_temp_max, char *loc3_temp_min, char *loc3_icon)
{
    bool f = 0;
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

            if (cnt == 7)
            {
                Serial.println(F("Can't connect to WIFI, restart initiated."));
                delay(100);
                ESP.restart();
            }
        }
    }

    // Wake up if sleeping and save inital state
    bool sleep = WiFi.getSleep();
    WiFi.setSleep(false);

    // Temporary
    char friendly_name[24]; // Not used, too many variables
    char unit[8]; // Not used, too many variables

    Serial.println(F("Initiating data collection from HA server"));

    // Fetching sensors data
    getSensorData("cuisine_pression", friendly_name, sensor1_press, unit);
    getSensorData("cuisine_humidite", friendly_name, sensor1_hum, unit);
    getSensorData("cuisine_temperature", friendly_name, sensor1_temp, unit);
    
    // Fetching sensors data
    getSensorData("chambre_pression", friendly_name, sensor2_press, unit);
    getSensorData("chambre_humidite", friendly_name, sensor2_hum, unit);
    getSensorData("chambre_temperature", friendly_name, sensor2_temp, unit);
    
    // Fetching sensors data
    getSensorData("grange_pression", friendly_name, sensor3_press, unit);
    getSensorData("grange_humidite", friendly_name, sensor3_hum, unit);
    getSensorData("grange_temperature", friendly_name, sensor3_temp, unit);

    // Miscellaneous, using custom HA sensors
    getState("sensor.time_formatted", currentTime);
    getState("sensor.nextsunrise", nextSunrise);
    getState("sensor.nextsunset", nextSunset);
    getWeatherHome(currentTemp, currentWind, currentWeather, currentWeatherIcon, expectedRain,
                    today_temp_max, today_temp_min, today_icon, tomorr_temp_max, tomorr_temp_min, tomorr_icon);

    // Forecasts
    getWeatherForecast("weather.villeurbanne", 0, loc1_temp_max, loc1_temp_min, loc1_icon);
    getWeatherForecast("weather.suhr", 0, loc2_temp_max, loc2_temp_min, loc2_icon);
    getWeatherForecast("weather.chambly", 0, loc3_temp_max, loc3_temp_min, loc3_icon);
    

    // TODO: Check to be integrated
    f = 0;

    // Return to initial state
    WiFi.setSleep(sleep);

    return !f;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////



// Gets the state and save is in output
void Network::getState(char entityName[], char *output)
{
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
    http.addHeader("Authorization",  HAtoken);

    // Actually do request
    int httpCode = http.GET();
    //Serial.println(httpCode);
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
            // Extracting Info from JSON
            strcpy(output, doc["state"].as<char *>());
        }
    }
    else if (httpCode == 401)
    {
        display.clean();
        display.setCursor(50, 290);
        display.setTextSize(3);
        display.print(F("Network error, probably wrong api key"));
        display.display();
        while (1);
    }

    // Stop http and clear document
    doc.clear();
    http.end();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////


void Network::getSensorData(char entityName[], char *friendlyName, char *value, char *unit)
{
    // Http object used to make get request
    HTTPClient http;

    // Initialization
    http.getStream().setNoDelay(true);
    http.getStream().setTimeout(1);
    char url[256];
    sprintf(url, "%s/states/sensor.%s", HAserverAPI, entityName);

    // Initiate http
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization",  HAtoken);

    // Actually do request
    int httpCode = http.GET();
    //Serial.println(httpCode);
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
            // Extracting Info from JSON
            strcpy(friendlyName, doc["attributes"]["friendly_name"].as<char *>());
            strcpy(value, doc["state"].as<char *>());
            strcpy(unit, doc["attributes"]["unit_of_measurement"].as<char *>());
        }
    }
    else if (httpCode == 401)
    {
        display.clean();
        display.setCursor(50, 290);
        display.setTextSize(3);
        display.print(F("Network error 401, probably wrong api key"));
        display.display();
        while (1);
    }

    // Stop http and clear document
    doc.clear();
    http.end();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

void Network::getWeatherHome(char *currentTemp, char *currentWind, char *currentWeather, char *currentWeatherIcon, char *expectedRain, 
                              char *today_temp_max, char *today_temp_min, char *today_icon, char *tomorr_temp_max, char *tomorr_temp_min, char *tomorr_icon)
{
    // Http object used to make get request
    HTTPClient http;

    // Initialization
    http.getStream().setNoDelay(true);
    http.getStream().setTimeout(1);
    char url[256];
    sprintf(url, "%s/states/weather.val_d_epy", HAserverAPI);

    // Initiate http
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization",  HAtoken);

    // Actually do request
    int httpCode = http.GET();
    //Serial.println(httpCode);
    if (httpCode == 200)
    {
        // Try parsing JSON object
        DeserializationError error = deserializeJson(doc, http.getStream());

        // If an error happens print it to Serial monitor
        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
        }
        else
        {
            //Current conditions
            strcpy(currentWeather, doc["state"].as<char *>());
            dtostrf(doc["attributes"]["temperature"].as<double>(), 4, 1, currentTemp);
            dtostrf(doc["attributes"]["wind_speed"].as<double>(), 4, 1, currentWind);
            dtostrf(doc["attributes"]["forecast"][0]["precipitation"].as<double>(), 4, 1, expectedRain);
            iconAbbr(currentWeatherIcon, doc["state"].as<char *>());
            
            // Forecast today
            dtostrf(doc["attributes"]["forecast"][0]["temperature"].as<double>(), 4, 1, today_temp_max);
            dtostrf(doc["attributes"]["forecast"][0]["templow"].as<double>(), 4, 1, today_temp_min);
            iconAbbr(today_icon, doc["attributes"]["forecast"][0]["condition"].as<char *>());
            
            // Forecast tomorrow
            dtostrf(doc["attributes"]["forecast"][1]["temperature"].as<double>(), 4, 1, tomorr_temp_max);
            dtostrf(doc["attributes"]["forecast"][1]["templow"].as<double>(), 4, 1, tomorr_temp_min);
            iconAbbr(tomorr_icon, doc["attributes"]["forecast"][1]["condition"].as<char *>());
        }
    }
    else if (httpCode == 401)
    {
        display.clean();
        display.setCursor(50, 290);
        display.setTextSize(3);
        display.print(F("Network error 401, probably wrong api key"));
        display.display();
        while (1);
    }

    // Stop http and clear document
    doc.clear();
    http.end();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

void Network::getWeatherForecast(char entityName[], int indForecast, char *forecastTemperatureMax, char *forecastTemperatureMin, char *forecastIcon)
{
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
    http.addHeader("Authorization",  HAtoken);

    // Actually do request
    int httpCode = http.GET();
    //Serial.println(httpCode);
    if (httpCode == 200)
    {
        // Try parsing JSON object
        DeserializationError error = deserializeJson(doc, http.getStream());

        // If an error happens print it to Serial monitor
        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
        }
        else
        {
            // Extracting Info from JSON
            dtostrf(doc["attributes"]["forecast"][indForecast]["temperature"].as<double>(), 4, 1, forecastTemperatureMax);
            dtostrf(doc["attributes"]["forecast"][indForecast]["templow"].as<double>(), 4, 1, forecastTemperatureMin);
            iconAbbr(forecastIcon, doc["state"].as<char *>());
        }
    }
    else if (httpCode == 401)
    {
        display.clean();
        display.setCursor(50, 290);
        display.setTextSize(3);
        display.print(F("Network error 401, probably wrong api key"));
        display.display();
        while (1);
    }

    // Stop http and clear document
    doc.clear();
    http.end();
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
