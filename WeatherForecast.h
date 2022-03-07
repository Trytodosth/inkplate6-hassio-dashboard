#ifndef WeatherForecast_h
#define WeatherForecast_h

class WeatherForecast
{
public:
    WeatherForecast();
    char Day[10];

    char Weather[20];
    char TemperatureMax[6];
    char TemperatureMin[6];
    char RainHeight[5];
    char WindSpeed[4];

    void setDay(int weekDay);

    void setDay(const char *value);
    void setDayFromDate(const char *dateStr);
    void setWeather(const char *value);
    void setTemperatureMax(const char *value);
    void setTemperatureMin(const char *value);
    void setRainHeight(const char *value);
    void setWindSpeed(const char *value);

    void setTemperatureMax(float value);
    void setTemperatureMin(float value);
    void setRainHeight(float value);
    void setWindSpeed(float value);

    void populateRandom();
    int dayofweek(int d, int m, int y);

private:
    char DaysWeek[7][12] = {"Dimanche", "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi"};
};

#endif