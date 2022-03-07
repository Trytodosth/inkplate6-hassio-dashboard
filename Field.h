#ifndef Field_h
#define Field_h

class Field
{
public:
    Field();
    Field(const char *name, const char *apiId, unsigned int lifetimeMinutes);
    char Name[15];
    char ApiID[40];

    char Value[10];

    void setName(const char *name);
    void setApiID(const char *apiId);
    void setValue(const char *valueSensor);
    void setLifetimeMinutes(unsigned int lifetime);
    
    bool updateNeeded();

private:
    unsigned long Lifetime = 60UL * 60000UL; // Update frequency [ms] set at 60 min by defaut
    unsigned long LastUpdate = 0; // millis()
};

#endif