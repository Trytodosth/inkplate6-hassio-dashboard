#ifndef City_h

#include "WeatherForecast.h"
#include "WeatherNow.h"
#define City_h

class City
{
public:
    City();
    City(char *name, char *apiId, float timeOffset);

    char Name[10];
    char ApiID[40];

    float TimeOffset = 0;
    WeatherNow ActualWeather;
    WeatherForecast Forecasts[4];

    void setName(char *name);
    void setApiID(char *apiId);
    void setTimeOFfset(float timeOffset);

    void setWeatherForecast(int index, int day, char *weather, float tempMax, float tempMin, float rain, float wind);

    bool updateNeeded();
    void populateRandom();

private:
    unsigned long Lifetime = 30UL * 60000UL; // Update frequency [ms] set at 30 min by defaut
    unsigned long LastUpdate = 0; // millis()
};

#endif