#ifndef Sensor_h
#define Sensor_h

class Sensor
{
public:
    Sensor(int typeSensor);
    char Unit[5];
    char Value[6];
    void setValue(char *valueSensor);
    void setValue(float valueSensor);

private:
    int typeSensor;
};

#endif