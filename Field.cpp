#include "Field.h"
#include <cstring>
#include <stdio.h>
#include <Arduino.h> // For millis()

Field::Field()
{
}

Field::Field(const char *name, const char *apiId, unsigned int lifetimeMinutes)
{
    setName(name);
    setApiID(apiId);
    setLifetimeMinutes(lifetimeMinutes);
}

void Field::setName(const char *name)
{
    strcpy(Name, name);
}
void Field::setApiID(const char *apiId)
{
    strcpy(ApiID, apiId);
}
void Field::setValue(const char *value)
{
    strcpy(Value, value);
    LastUpdate = millis();
}

bool Field::updateNeeded()
{
    if ((LastUpdate == 0) || ((millis() - LastUpdate) > Lifetime)) // milliseconds to minutes
    {
        return true;
    }
    return false;
}

void Field::setLifetimeMinutes(unsigned int lifetime)
{
    Lifetime = (unsigned long)lifetime * 60000UL; // From minutes to milliseconds
}