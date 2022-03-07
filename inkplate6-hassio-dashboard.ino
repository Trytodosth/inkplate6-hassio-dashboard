#include "Inkplate.h"
#include "Sensor.h"
#include "Room.h"
#include "WeatherForecast.h"
#include "WeatherNow.h"
#include "City.h"
#include "Helpers.h"
#include "Network.h"
#include "Field.h"

// Including fonts used
#include "Fonts/Roboto_Light_24.h"
#include "Fonts/Roboto_Light_36.h"
#include "Fonts/Roboto_Light_48.h"
#include "Fonts/Roboto_Light_72.h"

Inkplate display(INKPLATE_1BIT); // Resolution 1024 x 758
Network network;                 // Network object

Room Rooms[3];
WeatherForecast Forecasts[4];
City Cities[4];
Field Sunrise;
Field Sunset;
Field TimeServer;

void setup()
{
    Serial.begin(115200);

    display.begin();
    display.setTextWrap(false);
    display.setFrontlight(1);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);

    // Initialize touchscreen
    if (!display.tsInit(true))
        Serial.println("Touchscreen init failed!");

    display.clearDisplay();
    display.setCursor(200, 200);
    display.setFont(&Roboto_Light_72);
    display.print(F("It's getting quite long"));
    display.setCursor(200, 280);
    display.setFont(&Roboto_Light_36);
    display.print(F("That's what she said"));
    display.display();

    // Initialize objects
    Cities[0] = City("Epy", "weather.val_d_epy", 0);
    Cities[1] = City("Suhr", "weather.suhr", 0);
    Cities[2] = City("Lyon", "weather.villeurbanne", 0);
    Cities[3] = City("Chambly", "weather.chambly", -6);

    Rooms[0] = Room("Cuisine", "sensor.cuisine_temperature", "sensor.cuisine_pression", "sensor.cuisine_humidite");
    Rooms[1] = Room("Salon", "sensor.salon_temperature", "sensor.salon_pression", "sensor.salon_humidite");
    Rooms[2] = Room("Chambre", "sensor.chambre_temperature", "sensor.chambre_pression", "sensor.chambre_humidite");

    Sunrise = Field("Sunrise", "sensor.nextsunrise", 60 * 12);
    Sunset = Field("Sunset", "sensor.nextsunset", 60 * 12);
    TimeServer = Field("Time", "sensor.time_formatted", 1);

    display.setCursor(200, 420);
    display.setFont(&Roboto_Light_24);
    display.print(F("Activating your 5G-Moderna chip"));
    display.partialUpdate();

    // Initiating Network
    network.begin(); // Before the delay: hidden waiting

    // Call main draw function defined below
}

void loop()
{
    Serial.println("Fetching Fields");
    updateFields();
    Serial.println("Fetching Weathers");
    updateWeathers();
    Serial.println("Fetching Sensors");
    updateSensors();
    
    Serial.println("Making it nice");
    displayHomePage();

    delay(10000);
}

void updateFields()
{
    network.getField(Sunrise);
    network.getField(Sunset);
    network.getField(TimeServer);
}
void updateWeathers()
{
    for (int i = 0; i < 4; i++)
    {
        network.getCityWeather(Cities[i]);
    }
}
void updateSensors()
{
    for (int i = 0; i < 3; i++)
    {
        network.getRoomData(Rooms[i]);
    }
}

void displayHomePage()
{
    Serial.print("Drawing homepage...");
    display.clearDisplay(); // Clear content in frame buffer

    drawWeatherNow(100, 150, Cities[0].ActualWeather);

    for (size_t i = 1; i < 4; i++)
    {
        drawForecastSmall(750, -110 + i * 160, Cities[i].Forecasts[0], Cities[i].Name);
        if (i > 1)
        {
            display.drawFastHLine(800, -120 + i * 160, 150, BLACK);
        }
    }
    
    for (int i = 0; i < 3; i++)
    {
        drawRoom(650, 550 + 70 * i, Rooms[i]);
    }

    drawSunriseIcon(310, 380);
    drawField(370, 410, Sunrise);

    drawSunsetIcon(310, 450);
    drawField(370, 480, Sunset);

    drawField(20, 50, TimeServer);

    display.setCursor(150, 100);
    display.setFont(&Roboto_Light_72);
    display.print(F("Val-d'Epy"));

    display.display();
    Serial.println(" Done");
}

void drawRoom(int X, int Y, const Room &room)
{
    display.setFont(&Roboto_Light_36);

    display.setCursor(X, Y);
    display.print(room.Name);

    display.setCursor(X + 190, Y);
    display.print(room.Temperature.Value);
    moveCursor(2, -12);
    printCelsiusAtCursor(&Roboto_Light_24, 24);
}

void drawField(int X, int Y, const Field &field)
{
    display.setFont(&Roboto_Light_36);
    display.setCursor(X, Y);
    display.print(field.Value);
}

void displayCityForecast()
{
    display.clearDisplay(); // Clear content in frame buffer

    for (size_t i = 0; i < 4; i++)
    {
        drawForecastFull(37 + 250 * i, 180, Cities[1].Forecasts[i]);
    }

    display.display();
}

void displayCities()
{
    display.clearDisplay(); // Clear content in frame buffer

    for (size_t i = 0; i < 4; i++)
    {
        Cities[i].populateRandom();
        drawForecastFull(37 + 250 * i, 180, Cities[i].Forecasts[0]);
    }

    display.display();
}

void displayForecasts()
{
    display.clearDisplay(); // Clear content in frame buffer

    display.setCursor(37, 80);
    display.setFont(&Roboto_Light_72);
    display.print("Meteo a Epy");

    for (size_t i = 0; i < 4; i++)
    {
        Forecasts[i].populateRandom();
        drawForecastSmall(37 + 250 * i, 180, Forecasts[i], Forecasts[i].Day);
    }

    display.display();
}

void drawForecastFull(int X, int Y, WeatherForecast &forecast)
{
    display.setCursor(X, Y);
    display.setFont(&Roboto_Light_48);
    display.print(forecast.Day);
    drawWeatherIcon_medium(X, Y + 50, forecast.Weather);
    // display.setCursor(X, 200);
    // display.println(forecast.Weather);

    display.setCursor(X, Y + 330);
    display.setFont(&Roboto_Light_72);
    display.print(forecast.TemperatureMax);
    display.setFont(&Roboto_Light_24);
    moveCursor(2, -30);
    printCelsiusAtCursor(&Roboto_Light_24, 24);

    display.setCursor(X, Y + 390);
    display.setFont(&Roboto_Light_48);
    display.print(forecast.TemperatureMin);
    moveCursor(2, -20);
    printCelsiusAtCursor(&Roboto_Light_24, 24);

    display.setFont(&Roboto_Light_36);
    drawRaindropIcon(X, Y + 420);
    display.setCursor(X + 60, Y + 460);
    display.print(forecast.RainHeight);
    display.setFont(&Roboto_Light_24);
    display.print(F("mm"));

    display.setFont(&Roboto_Light_36);
    drawWindIcon(X, Y + 480);
    display.setCursor(X + 60, Y + 515);
    display.print(forecast.WindSpeed);
    display.setFont(&Roboto_Light_24);
    display.print(F("km/h"));
}

void drawForecastSmall(int X, int Y, WeatherForecast &forecast, char *title)
{
    drawWeatherIcon_small(X, Y + 30, forecast.Weather);

    display.setFont(&Roboto_Light_36);
    display.setCursor(X + 85, Y + 36);
    display.print(title);

    display.setFont(&Roboto_Light_36);
    display.setCursor(X + 110, Y + 80);
    display.print(forecast.TemperatureMax);
    moveCursor(2, -12);
    printCelsiusAtCursor(&Roboto_Light_24, 24);

    display.setFont(&Roboto_Light_36);
    display.setCursor(X + 110, Y + 124);
    display.print(forecast.TemperatureMin);
    moveCursor(2, -12);
    printCelsiusAtCursor(&Roboto_Light_24, 24);
}

void drawWeatherNow(int X, int Y, WeatherNow &weather)
{
    drawWeatherIcon_medium(X, Y, weather.Weather);

    display.setCursor(X, Y + 280);
    display.setFont(&Roboto_Light_72);
    display.print(weather.Temperature);
    display.setFont(&Roboto_Light_24);
    moveCursor(2, -30);
    printCelsiusAtCursor(&Roboto_Light_24, 24);

    display.setFont(&Roboto_Light_36);
    drawWindIcon(X, Y + 310);
    display.setCursor(X + 60, Y + 350);
    display.print(weather.WindSpeed);
    display.setFont(&Roboto_Light_24);
    display.print(F("KM/H"));
}