#ifndef WeatherNow_h
#define WeatherNow_h

class WeatherNow
{
public:
    WeatherNow();

    char Weather[20];
    char Temperature[6];
    char WindSpeed[4];

    void setWeather(const char *value);
    void setTemperature(const char *value);
    void setWindSpeed(const char *value);

    void setTemperature(float value);
    void setWindSpeed(float value);

    void populateRandom();

private:
};

#endif