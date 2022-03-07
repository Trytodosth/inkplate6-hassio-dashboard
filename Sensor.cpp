#include "Sensor.h"
#include <cstring>
#include <stdio.h>

Sensor::Sensor(int typeSensor)
{
    switch (typeSensor)
    {
    case 1:
        strcpy(Unit, "deg");
        break;
    case 2:
        strcpy(Unit, "hPa");
        break;
    case 3:
        strcpy(Unit, "%hum");
        break;
    }
}

void Sensor::setValue(char *valueSensor)
{
    strcpy(Value, valueSensor);
}
void Sensor::setValue(float valueSensor)
{
    int ret = snprintf(Value, sizeof Value, "%f", valueSensor);
}