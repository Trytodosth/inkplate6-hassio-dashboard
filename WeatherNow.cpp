#include "WeatherNow.h"
#include "Helpers.h"
#include <cstring>
#include <stdio.h>

WeatherNow::WeatherNow()
{
}

// Input as char array
void WeatherNow::setWeather(const char *value)
{
    strcpy(Weather, value);
}
void WeatherNow::setTemperature(const char *value)
{
    strcpy(Temperature, value);
}
void WeatherNow::setWindSpeed(const char *value)
{
    strcpy(WindSpeed, value);
}

// Input as float
void WeatherNow::setTemperature(float value)
{
    int ret = snprintf(Temperature, sizeof Temperature, "%.1f", value);
}
void WeatherNow::setWindSpeed(float value)
{
    int ret = snprintf(WindSpeed, sizeof WindSpeed, "%.0f", value);
}

void WeatherNow::populateRandom()
{
    setWeather(abbrs[random(15)]);
    setTemperature(random(500) * 0.1 - 20);
    setWindSpeed(random(1500) * 0.1);
}
