#include "WeatherForecast.h"
#include "Helpers.h"
#include <cstring>
#include <stdio.h>
#include <bits/stdc++.h>

WeatherForecast::WeatherForecast()
{
}

void WeatherForecast::setDay(int weekDay)
{
    strcpy(Day, DaysWeek[weekDay]);
}

// Input as char array
void WeatherForecast::setDay(const char *value)
{
    if (value) // Check if null (empty JSON)
    {
        strcpy(Day, value);
    }
    else
    {
        Serial.println("Error: Invalid assignment to 'Day'!");
    }
}
void WeatherForecast::setDayFromDate(const char *dateStr)
{
    if (dateStr) // Check if null (empty JSON)
    {
        char tmpStr[30];
        char yearStr[5];
        char monthStr[3];
        char dayStr[3];
        char *pch;
        int year;
        int month;
        int day;

        // Serial.print("Trying to parse ");
        // Serial.println(dateStr);

        // Splitting string into tokens
        strcpy(tmpStr, dateStr);
        pch = strtok(tmpStr, "-T");
        if (pch != NULL)
        {
            strcpy(yearStr, pch);
            pch = strtok(NULL, "-T");
        }
        if (pch != NULL)
        {
            strcpy(monthStr, pch);
            pch = strtok(NULL, "-T");
        }
        if (pch != NULL)
        {
            strcpy(dayStr, pch);
            pch = strtok(NULL, "-T");
        }

        // Converting to integers
        try
        {
            year = atoi(yearStr);
            month = atoi(monthStr);
            day = atoi(dayStr);
        }
        catch (std::exception const &e)
        {
            // This could not be parsed into a number so an exception is thrown.
            // atoi() would return 0, which is less helpful if it could be a valid value.
            return;
        }

        // Serial.println(year);
        // Serial.println(month);
        // Serial.println(day);
        int weekDay = dayofweek(day, month, year);
        strcpy(Day, DaysWeek[weekDay]);
        // Serial.println(Day);
    }
    else
    {
        Serial.println("Error: Invalid assignment to 'Day'!");
    }
}
void WeatherForecast::setWeather(const char *value)
{
    if (value) // Check if null (empty JSON)
    {
        strcpy(Weather, value);
    }
    else
    {
        Serial.println("Error: Invalid assignment to 'Weather'!");
    }
}
void WeatherForecast::setTemperatureMax(const char *value)
{
    strcpy(TemperatureMax, value);
}
void WeatherForecast::setTemperatureMin(const char *value)
{
    strcpy(TemperatureMin, value);
}
void WeatherForecast::setRainHeight(const char *value)
{
    strcpy(RainHeight, value);
}
void WeatherForecast::setWindSpeed(const char *value)
{
    strcpy(WindSpeed, value);
}

// Input as float
void WeatherForecast::setTemperatureMax(float value)
{
    int ret = snprintf(TemperatureMax, sizeof TemperatureMax, "%.1f", value);
}
void WeatherForecast::setTemperatureMin(float value)
{
    int ret = snprintf(TemperatureMin, sizeof TemperatureMin, "%.1f", value);
}
void WeatherForecast::setRainHeight(float value)
{
    int ret = snprintf(RainHeight, sizeof RainHeight, "%.1f", value);
}
void WeatherForecast::setWindSpeed(float value)
{
    int ret = snprintf(WindSpeed, sizeof WindSpeed, "%.0f", value);
}

void WeatherForecast::populateRandom()
{
    setDay(random(7));
    setWeather(abbrs[random(15)]);
    setTemperatureMax(random(300) * 0.1);
    setTemperatureMin(random(200) * 0.1 - 10);
    setRainHeight(random(100) * 0.1);
    setWindSpeed(random(1500) * 0.1);
}

int WeatherForecast::dayofweek(int d, int m, int y)
{
    // From: https://www.geeksforgeeks.org/find-day-of-the-week-for-a-given-date/
    // static int t[] = { 0, 3, 2, 5, 0, 3,
    //                    5, 1, 4, 6, 2, 4 };
    // y -= m < 3;
    // return ( y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;

    // Also: https://stackoverflow.com/questions/6054016/c-program-to-find-day-of-week-given-date
    return (d += m < 3 ? y-- : y - 2, 23 * m / 9 + d + 4 + y / 4 - y / 100 + y / 400) % 7;
}