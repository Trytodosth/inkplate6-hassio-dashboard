#include "Room.h"
#include <cstring>
#include <stdio.h>

Room::Room() : Temperature(1), Pressure(2), Humidity(3)
{
}

Room::Room(char *name, char *apiIdTemperature, char *apiIdPressure, char *apiIdHumidity) : Temperature(1), Pressure(2), Humidity(3)
{
    setName(name);
    setApiID(apiIdTemperature, 1);
    setApiID(apiIdPressure, 2);
    setApiID(apiIdHumidity, 3);
}

// Input as char array
void Room::setName(const char *name)
{
    strcpy(Name, name);
}
void Room::setApiID(const char *apiId, int indexSensor)
{
    switch (indexSensor)
    {
    case 1:
        strcpy(ApiIdTemperature, apiId);
        break;
    case 2:
        strcpy(ApiIdPressure, apiId);
        break;
    case 3:
        strcpy(ApiIdHumidity, apiId);
        break;
    }
}
void Room::setTemperature(const char *valueSensor)
{
    strcpy(Temperature.Value, valueSensor);
}
void Room::setPressure(const char *valueSensor)
{
    strcpy(Pressure.Value, valueSensor);
}
void Room::setHumidity(const char *valueSensor)
{
    strcpy(Humidity.Value, valueSensor);
}

// Input as float
void Room::setTemperature(float valueSensor)
{
    int ret = snprintf(Temperature.Value, sizeof Temperature.Value, "%f", valueSensor);
}
void Room::setPressure(float valueSensor)
{
    int ret = snprintf(Pressure.Value, sizeof Pressure.Value, "%f", valueSensor);
}
void Room::setHumidity(float valueSensor)
{
    int ret = snprintf(Humidity.Value, sizeof Humidity.Value, "%f", valueSensor);
}