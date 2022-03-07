#ifndef Room_h
#define Room_h

#include "Sensor.h"

class Room
{
public:
    Room();
    Room(char *name, char *apiIdTemperature, char *apiIdPressure, char *apiIdHumidity);
    char Name[10];
    char ApiIdTemperature[40];
    char ApiIdPressure[40];
    char ApiIdHumidity[40];

    Sensor Temperature;
    Sensor Pressure;
    Sensor Humidity;
    
    void setName(const char *name);
    void setApiID(const char *apiId, int indexSensor);
    void setTemperature(const char *valueSensor);
    void setPressure(const char *valueSensor);
    void setHumidity(const char *valueSensor);
    
    void setTemperature(float valueSensor);
    void setPressure(float valueSensor);
    void setHumidity(float valueSensor);

private:
};

#endif