#include "Room.h"
#include <cstring>
#include <stdio.h>

Room::Room() : Temperature(1), Pressure(2), Humidity(3)
{
}

Room::Room(char *nameRoom) : Temperature(1), Pressure(2), Humidity(3)
{
    strcpy(Name, nameRoom);
}

// Input as char array
void Room::setTemperature(char *valueSensor)
{
    strcpy(Temperature.Value, valueSensor);
}
void Room::setPressure(char *valueSensor)
{
    strcpy(Pressure.Value, valueSensor);
}
void Room::setHumidity(char *valueSensor)
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