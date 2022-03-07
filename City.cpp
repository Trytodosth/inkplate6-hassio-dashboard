#include "City.h"
#include "WeatherForecast.h"
#include "WeatherNow.h"
#include <cstring>
#include <stdio.h>
#include <Arduino.h> // For millis()

City::City()
{
}

City::City(char *name, char *apiId, float timeOffset)
{
    setName(name);
    setApiID(apiId);
    setTimeOFfset(timeOffset);
}

// Input as char array
void City::setName(char *name)
{
    strcpy(Name, name);
}
void City::setApiID(char *apiId)
{
    strcpy(ApiID, apiId);
}
void City::setTimeOFfset(float timeOffset)
{
    TimeOffset = timeOffset;
}

void City::setWeatherForecast(int i, int day, char *weather, float tempMax, float tempMin, float rain, float wind)
{
    Forecasts[i].setDay(day);
    Forecasts[i].setWeather(weather);
    Forecasts[i].setTemperatureMax(tempMax);
    Forecasts[i].setTemperatureMin(tempMin);
    Forecasts[i].setRainHeight(rain);
    Forecasts[i].setWindSpeed(wind);
}



bool City::updateNeeded()
{
    if ((LastUpdate == 0) || ((millis() - LastUpdate) > Lifetime)) // milliseconds to minutes
    {
        return true;
    }
    return false;
}


void City::populateRandom()
{
    for (size_t i = 0; i < 4; i++)
    {
        Forecasts[i].populateRandom();
    }
}
