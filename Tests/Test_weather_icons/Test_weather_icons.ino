/*
  Very simple program to review the weather icons
  The icons must be 1 bit.
*/

// Resources
#include "Inkplate.h"
#include "icons.h"
#include "Fonts/Roboto_Light_36.h"
#include "Fonts/Roboto_Light_48.h"


// Contants used for drawing icons
char abbrs[15][24] = {"clear-night", "cloudy", "fog", "hail", "lightning", "lightning-rainy", "partlycloudy",
                      "pouring", "rainy", "snowy", "snowy-rainy", "sunny", "windy", "windy-variant", "exceptional"
                     };

// Logo associated with keywords. You can use several times the same (here windy-variant = windy)
const uint8_t *logos_100px[15] = {
  icon_clear_night_100px, icon_cloudy_100px, icon_fog_100px, icon_hail_100px, icon_lightning_rainy_100px, icon_lightning_100px, icon_partlycloudy_100px,
  icon_pouring_100px, icon_rainy_100px, icon_snowy_100px, icon_snowy_rainy_100px, icon_sunny_100px, icon_windy_100px, icon_windy_100px, icon_exceptional_100px
};
const uint8_t *logos_200px[15] = {
  icon_clear_night_200px, icon_cloudy_200px, icon_fog_200px, icon_hail_200px, icon_lightning_rainy_200px, icon_lightning_200px, icon_partlycloudy_200px,
  icon_pouring_200px, icon_rainy_200px, icon_snowy_200px, icon_snowy_rainy_200px, icon_sunny_200px, icon_windy_200px, icon_windy_200px, icon_exceptional_200px
};


Inkplate display(INKPLATE_1BIT);

void setup()
{
  // Initializing
  Serial.begin(115200);
  display.begin();
  display.clearDisplay(); // Clear Buffer

  // Text
  display.setTextColor(BLACK);
  display.setFont(&Roboto_Light_48);
  display.setCursor(200, 200);
  display.print(F("Weather icon check"));
  display.setFont(&Roboto_Light_36);
  display.setCursor(200, 260);
  display.print(F("Looping through the icons"));
  display.setCursor(200, 310);
  display.print(F("Mode 1 bit, full refresh"));
 
  display.display();
  delay(5000);
}


void loop()
{
  // 200x200 px
  for (int i = 0; i < 15; ++i)
  {
    display.clearDisplay(); // Clear Buffer
    display.setTextColor(BLACK);
    display.setFont(&Roboto_Light_48);
    display.setCursor(250, 150);
    display.print(abbrs[i]);
    display.setFont(&Roboto_Light_36);
    display.setCursor(250, 200);
    display.print(F("200x200 px"));
    
    display.drawBitmap(250, 230, logos_200px[i], 200, 200, BLACK);

    display.display();
    delay(1500);
  }

  // 100x100 px
  for (int i = 0; i < 15; ++i)
  {
    display.clearDisplay(); // Clear Buffer
    display.setTextColor(BLACK);
    display.setFont(&Roboto_Light_48);
    display.setCursor(250, 150);
    display.print(abbrs[i]);
    display.setFont(&Roboto_Light_36);
    display.setCursor(250, 200);
    display.print(F("100x100 px"));
    
    display.drawBitmap(250, 230, logos_100px[i], 100, 100, BLACK);

    display.display();
    delay(1500);
  }
}
