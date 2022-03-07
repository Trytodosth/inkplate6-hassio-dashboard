#ifndef Room_h
#define Room_h

#include "Sensor.h"

class Room
{
public:
    Room();
    Room(char *nameRoom);
    char Name[10];
    Sensor Temperature;
    Sensor Pressure;
    Sensor Humidity;
    void setTemperature(char *valueSensor);
    void setPressure(char *valueSensor);
    void setHumidity(char *valueSensor);
    
    void setTemperature(float valueSensor);
    void setPressure(float valueSensor);
    void setHumidity(float valueSensor);

private:
};

#endif